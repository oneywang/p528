// Copyright by wangtianping. All rights reserved.

#include "chrome/app/client_util.h"

#include <windows.h>
#include <shlwapi.h>

#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/environment.h"
#include "base/file_version_info.h"
#include "base/files/file_util.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/path_service.h"
#include "base/strings/string16.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/trace_event/trace_event.h"
#include "base/version.h"
#include "base/win/scoped_handle.h"
#include "base/win/windows_version.h"
#include "breakpad/src/client/windows/handler/exception_handler.h"
#include "chrome/app/chrome_crash_reporter_client.h"
#include "chrome/app/client_util.h"
#include "chrome/app/chrome_env_vars.h"
#include "chrome/app/image_pre_reader_win.h"
#include "chrome/app/ini_parser.h"
#include "chrome/crashreport/breakpad_win.h"
#include "content/content_switches.h"
#include "content/result_codes.h"

namespace {
// The entry point signature of chrome.dll.
  typedef int(__cdecl *DLL_MAIN)(HINSTANCE, void*);

base::LazyInstance<chrome::ChromeCrashReporterClient>::Leaky
    g_chrome_crash_client = LAZY_INSTANCE_INITIALIZER;

// Loads |module| after setting the CWD to |module|'s directory. Returns a
// reference to the loaded module on success, or null on error.
HMODULE LoadModuleWithDirectory(const base::FilePath& module, bool pre_read) {
  ::SetCurrentDirectoryW(module.DirName().value().c_str());

  if (pre_read) {
    // We pre-read the binary to warm the memory caches (fewer hard faults to
    // page parts of the binary in).
    const size_t kStepSize = 1024 * 1024;
    size_t percent = 100;
    ImagePreReader::PartialPreReadImage(module.value().c_str(), percent,
                                        kStepSize);
  }

  return ::LoadLibraryExW(module.value().c_str(), nullptr,
                          LOAD_WITH_ALTERED_SEARCH_PATH);
}

// Returns the directory in which the currently running executable resides.
base::FilePath GetExecutableDir() {
  base::char16 path[MAX_PATH];
  ::GetModuleFileNameW(nullptr, path, MAX_PATH);
  return base::FilePath(path).DirName();
}

}  // namespace

base::string16 GetCurrentModuleVersion() {
  scoped_ptr<FileVersionInfo> file_version_info(
      FileVersionInfo::CreateFileVersionInfoForCurrentModule());
  if (file_version_info.get()) {
    base::string16 version_string(file_version_info->file_version());
    if (Version(base::UTF16ToASCII(version_string)).IsValid())
      return version_string;
  }
  return base::string16();
}

base::string16 GetAppConfig(const std::string& key, const base::string16& default_val){
  base::FilePath module_dir = GetExecutableDir();
  base::FilePath ini_path = module_dir.Append(L"chrome.ini");
  std::string ini_content;
  base::ReadFileToString(ini_path, &ini_content);
  DictionaryValueINIParser ini_parser;
  ini_parser.Parse(ini_content);
  const base::DictionaryValue& root = ini_parser.root();
  base::string16 val;
  root.GetString(key, &val);
  if (val.empty()){
    val = default_val;
  }
  return val;
}

//=============================================================================

MainDllLoader::MainDllLoader()
  : dll_(nullptr) {
}

MainDllLoader::~MainDllLoader() {
}

// Loading chrome is an interesting affair. First we try loading from the
// current directory to support run-what-you-compile and other development
// scenarios.
// If that fails then we look at the version resource in the current
// module. This is the expected path for chrome.exe browser instances in an
// installed build.
HMODULE MainDllLoader::Load(base::string16* version, base::FilePath* module) {
  base::string16 dll_name = GetAppConfig("dll.name",L"chrome.dll");

  const bool pre_read = true;
  base::FilePath module_dir = GetExecutableDir();
  *module = module_dir.Append(dll_name);
  HMODULE dll = LoadModuleWithDirectory(*module, pre_read);
  if (!dll) {
    base::string16 version_string(GetCurrentModuleVersion());
    if (version_string.empty()) {
      LOG(ERROR) << "No valid Chrome version found";
      return nullptr;
    }
    *version = version_string;
    *module = module_dir.Append(version_string).Append(dll_name);
    dll = LoadModuleWithDirectory(*module, pre_read);
    if (!dll) {
      PLOG(ERROR) << "Failed to load Chrome DLL from " << module->value();
      return nullptr;
    }
  }

  DCHECK(dll);
  return dll;
}

// Launching is a matter of loading the right dll, setting the CHROME_VERSION
// environment variable and just calling the entry point. Derived classes can
// add custom code in the OnBeforeLaunch callback.
int MainDllLoader::Launch(HINSTANCE instance) {
  const base::CommandLine& cmd_line = *base::CommandLine::ForCurrentProcess();
  process_type_ = cmd_line.GetSwitchValueASCII(switches::kProcessType);

  base::string16 version;
  base::FilePath file;

  crash_reporter::SetCrashReporterClient(g_chrome_crash_client.Pointer());
  bool exit_now = true;
  if (process_type_.empty()) {
    if (breakpad::ShowRestartDialogIfCrashed(&exit_now)) {
      // We restarted because of a previous crash. Ask user if we should
      // Relaunch. Only for the browser process. See crbug.com/132119.
      if (exit_now)
        return content::RESULT_CODE_NORMAL_EXIT;
    }
  }
  chrome::ChromeCrashReporterClient::PrepareRestartOnCrashEnviroment(cmd_line);
  breakpad::InitCrashReporter(process_type_);

  dll_ = Load(&version, &file);
  if (!dll_)
    return content::RESULT_CODE_MISSING_DATA;

  DLL_MAIN chrome_main =
      reinterpret_cast<DLL_MAIN>(::GetProcAddress(dll_, "ChromeMain"));
  int rc = chrome_main(instance,nullptr);
  // Sandboxed processes close some system DLL handles after lockdown so ignore
  // EXCEPTION_INVALID_HANDLE generated on Windows 10 during shutdown of these
  // processes.
  // TODO(wfh): Check whether MS have fixed this in Win10 RTM. crbug.com/456193
  if (base::win::GetVersion() >= base::win::VERSION_WIN10)
    breakpad::ConsumeInvalidHandleExceptions();
  return rc;
}

//=============================================================================

MainDllLoader* MakeMainDllLoader() {
  return new MainDllLoader();
}
