// Copyright by wangtianping. All rights reserved.

#include "chrome/crashreport/breakpad_win.h"

#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <userenv.h>
#include <winnt.h>

#include <algorithm>
#include <map>
#include <vector>

#include "base/base_switches.h"
#include "base/basictypes.h"
#include "base/command_line.h"
#include "base/debug/crash_logging.h"
#include "base/debug/dump_without_crashing.h"
#include "base/environment.h"
#include "base/files/file_util.h"
#include "base/memory/scoped_ptr.h"
#include "base/numerics/safe_conversions.h"
#include "base/strings/string16.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/synchronization/lock.h"
#include "base/win/metro.h"
#include "base/win/pe_image.h"
#include "base/win/registry.h"
#include "base/win/win_util.h"
#include "breakpad/src/client/windows/handler/exception_handler.h"
#include "chrome/crashreport/crash_reporter_client.h"
#include "chrome/crashreport/hard_error_handler_win.h"

namespace breakpad {

using crash_reporter::GetCrashReporterClient;

namespace {

// Minidump with stacks, PEB, TEB, and unloaded module list.
const MINIDUMP_TYPE kSmallDumpType = static_cast<MINIDUMP_TYPE>(
    MiniDumpWithProcessThreadData |  // Get PEB and TEB.
    MiniDumpWithHandleData |
    MiniDumpWithUnloadedModules);  // Get unloaded modules when available.

// Minidump with all of the above, plus memory referenced from stack.
const MINIDUMP_TYPE kLargerDumpType = static_cast<MINIDUMP_TYPE>(
    MiniDumpWithProcessThreadData |  // Get PEB and TEB.
    MiniDumpWithHandleData |
    MiniDumpWithUnloadedModules |  // Get unloaded modules when available.
    MiniDumpWithIndirectlyReferencedMemory);  // Get memory referenced by stack.

// Large dump with all process memory.
const MINIDUMP_TYPE kFullDumpType = static_cast<MINIDUMP_TYPE>(
    MiniDumpWithFullMemory |  // Full memory from process.
    MiniDumpWithProcessThreadData |  // Get PEB and TEB.
    MiniDumpWithHandleData |  // Get all handle information.
    MiniDumpWithUnloadedModules);  // Get unloaded modules when available.

google_breakpad::ExceptionHandler* g_breakpad = NULL;
google_breakpad::ExceptionHandler* g_dumphandler_no_crash = NULL;
}  // namespace

// Used for dumping a process state when there is no crash.
extern "C" void __declspec(dllexport) __cdecl DumpProcessWithoutCrash() {
  if (g_dumphandler_no_crash) {
    g_dumphandler_no_crash->WriteMinidump();
  }
}

namespace {
// Returns the directory in which the currently running executable resides.
base::FilePath GetExecutableDir() {
  base::char16 path[MAX_PATH];
  ::GetModuleFileNameW(nullptr, path, MAX_PATH);
  return base::FilePath(path).DirName();
}

// This callback is used when we want to get a dump without crashing the
// process.
bool DumpDoneCallbackWhenNoCrash(const wchar_t*, const wchar_t*, void*,
                                 EXCEPTION_POINTERS* ex_info,
                                 MDRawAssertionInfo*, bool succeeded) {
  GetCrashReporterClient()->RecordCrashDumpAttemptResult(
      false /* is_real_crash */, succeeded);
  return true;
}

// This callback is executed when the browser process has crashed, after
// the crash dump has been created. We need to minimize the amount of work
// done here since we have potentially corrupted process. Our job is to
// spawn another instance of chrome which will show a 'chrome has crashed'
// dialog. This code needs to live in the exe and thus has no access to
// facilities such as the i18n helpers.
bool DumpDoneCallback(const wchar_t*, const wchar_t*, void*,
                      EXCEPTION_POINTERS* ex_info,
                      MDRawAssertionInfo*, bool succeeded) {
  GetCrashReporterClient()->RecordCrashDumpAttemptResult(
      true /* is_real_crash */, succeeded);
  // Check if the exception is one of the kind which would not be solved
  // by simply restarting chrome. In this case we show a message box with
  // and exit silently. Remember that chrome is in a crashed state so we
  // can't show our own UI from this process.
  if (HardErrorHandler(ex_info))
    return true;

  if (!GetCrashReporterClient()->AboutToRestart())
    return true;

  // Now we just start chrome browser with the same command line.
  STARTUPINFOW si = {sizeof(si)};
  PROCESS_INFORMATION pi;
  if (::CreateProcessW(NULL, ::GetCommandLineW(), NULL, NULL, FALSE,
                       CREATE_UNICODE_ENVIRONMENT, NULL, NULL, &si, &pi)) {
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
  }
  // After this return we will be terminated. The actual return value is
  // not used at all.
  return true;
}

// flag to indicate that we are already handling an exception.
volatile LONG handling_exception = 0;

// This callback is used when there is no crash. Note: Unlike the
// |FilterCallback| below this does not do dupe detection. It is upto the caller
// to implement it.
bool FilterCallbackWhenNoCrash(
    void*, EXCEPTION_POINTERS*, MDRawAssertionInfo*) {
  GetCrashReporterClient()->RecordCrashDumpAttempt(false);
  return true;
}

// This callback is executed when the Chrome process has crashed and *before*
// the crash dump is created. To prevent duplicate crash reports we
// make every thread calling this method, except the very first one,
// go to sleep.
bool FilterCallback(void*, EXCEPTION_POINTERS*, MDRawAssertionInfo*) {
  // Capture every thread except the first one in the sleep. We don't
  // want multiple threads to concurrently report exceptions.
  if (::InterlockedCompareExchange(&handling_exception, 1, 0) == 1) {
    ::Sleep(INFINITE);
  }
  GetCrashReporterClient()->RecordCrashDumpAttempt(true);
  return true;
}
}  // namespace

static bool WrapMessageBoxWithSEH(const wchar_t* text, const wchar_t* caption,
                                  UINT flags, bool* exit_now) {
  // We wrap the call to MessageBoxW with a SEH handler because it some
  // machines with CursorXP, PeaDict or with FontExplorer installed it crashes
  // uncontrollably here. Being this a best effort deal we better go away.
  __try {
    *exit_now = (IDOK != ::MessageBoxW(NULL, text, caption, flags));
  } __except(EXCEPTION_EXECUTE_HANDLER) {
    // Its not safe to continue executing, exit silently here.
    ::TerminateProcess(::GetCurrentProcess(),
                       GetCrashReporterClient()->GetResultCodeRespawnFailed());
  }

  return true;
}

// This function is executed by the child process that DumpDoneCallback()
// spawned and basically just shows the 'chrome has crashed' dialog if
// the CHROME_CRASHED environment variable is present.
bool ShowRestartDialogIfCrashed(bool* exit_now) {
  // If we are being launched in metro mode don't try to show the dialog.
  if (base::win::IsMetroProcess())
    return false;

  base::string16 message;
  base::string16 title;
  bool is_rtl_locale;
  if (!GetCrashReporterClient()->ShouldShowRestartDialog(
          &title, &message, &is_rtl_locale)) {
    return false;
  }

  // If the UI layout is right-to-left, we need to pass the appropriate MB_XXX
  // flags so that an RTL message box is displayed.
  UINT flags = MB_OKCANCEL | MB_ICONWARNING;
  if (is_rtl_locale)
    flags |= MB_RIGHT | MB_RTLREADING;

  return WrapMessageBoxWithSEH(message.c_str(), title.c_str(), flags, exit_now);
}

void InitCrashReporter(const std::string& process_type_switch) {
  const base::CommandLine& command = *base::CommandLine::ForCurrentProcess();
  if (command.HasSwitch(switches::kDisableBreakpad))
    return;
#if 0
  // Disable the message box for assertions.(see setcrt)
  _CrtSetReportMode(_CRT_ASSERT, 0);

  // Preserve existing error mode, as discussed at http://t/dmea
  // see logging
  UINT new_flags = SEM_FAILCRITICALERRORS |
    SEM_NOGPFAULTERRORBOX |
    SEM_NOOPENFILEERRORBOX;
  UINT existing_flags = SetErrorMode(new_flags);
  SetErrorMode(existing_flags | new_flags);
#endif 

  std::string process_type = process_type_switch;
  if (process_type.empty())
    process_type = "browser";
  if (!GetCrashReporterClient()->EnableBreakpadForProcess(process_type))
    return;

  wchar_t exe_path[MAX_PATH];
  exe_path[0] = 0;
  GetModuleFileNameW(NULL, exe_path, MAX_PATH);

  bool is_per_user_install =
      GetCrashReporterClient()->GetIsPerUserInstall(base::FilePath(exe_path));

  google_breakpad::CustomClientInfo* custom_info = nullptr;
  google_breakpad::ExceptionHandler::MinidumpCallback callback = NULL;

  // We install the post-dump callback only for the browser and service
  // processes. It spawns a new browser/service process.
  callback = &DumpDoneCallback;
  GetCrashReporterClient()->InitBrowserCrashDumpsRegKey();

  std::string pipe_name_ascii;
  base::string16 pipe_name = base::ASCIIToUTF16(pipe_name_ascii);

  // current dir + dumps
  base::FilePath dumps_dir = GetExecutableDir().Append(L"dumps");
  base::CreateDirectory(dumps_dir);

  MINIDUMP_TYPE dump_type = kSmallDumpType;
  // Capture full memory if explicitly instructed to.
  if (command.HasSwitch(switches::kFullMemoryCrashReport))
    dump_type = kFullDumpType;
  else if (GetCrashReporterClient()->GetShouldDumpLargerDumps(
               is_per_user_install))
    dump_type = kLargerDumpType;

  g_breakpad = new google_breakpad::ExceptionHandler(dumps_dir.value(), &FilterCallback,
                   callback, NULL,
                   google_breakpad::ExceptionHandler::HANDLER_ALL,
                   dump_type, pipe_name.c_str(), custom_info);

  // Now initialize the non crash dump handler.
  g_dumphandler_no_crash = new google_breakpad::ExceptionHandler(dumps_dir.value(),
      &FilterCallbackWhenNoCrash,
      &DumpDoneCallbackWhenNoCrash,
      NULL,
      // Set the handler to none so this handler would not be added to
      // |handler_stack_| in |ExceptionHandler| which is a list of exception
      // handlers.
      google_breakpad::ExceptionHandler::HANDLER_NONE,
      dump_type, pipe_name.c_str(), custom_info);

  // Set the DumpWithoutCrashingFunction for this instance of base.lib.  Other
  // executable images linked with base should set this again for
  // DumpWithoutCrashing to function correctly.
  // See chrome_main.cc for example.
  base::debug::SetDumpWithoutCrashingFunction(&DumpProcessWithoutCrash);

  // Tells breakpad to handle breakpoint and single step exceptions.
  // This might break JIT debuggers, but at least it will always
  // generate a crashdump for these exceptions.
  g_breakpad->set_handle_debug_exceptions(true);
}

void ConsumeInvalidHandleExceptions() {
  if (g_breakpad) {
    g_breakpad->set_consume_invalid_handle_exceptions(true);
  }
  if (g_dumphandler_no_crash) {
    g_dumphandler_no_crash->set_consume_invalid_handle_exceptions(true);
  }
}

}  // namespace breakpad
