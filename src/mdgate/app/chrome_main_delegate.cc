// Copyright by wangtianping. All rights reserved.

#include "mdgate/app/chrome_main_delegate.h"

#include "base/base_paths.h"
#include "base/command_line.h"
#include "base/cpu.h"
#include "base/files/file_path.h"
#include "base/i18n/rtl.h"
#include "base/lazy_instance.h"
#include "base/message_loop/message_loop.h"
#include "base/metrics/statistics_recorder.h"
#include "base/path_service.h"
#include "base/process/memory.h"
#include "base/process/process_handle.h"
#include "base/strings/string_util.h"
#include "build/build_config.h"
#include "content/content_switches.h"
#include "mdgate/app/logging_chrome.h"
#include "mdgate/browser/chrome_content_browser_client.h"

namespace{

base::LazyInstance<chrome::ChromeContentBrowserClient>
    g_chrome_content_browser_client = LAZY_INSTANCE_INITIALIZER;

}  // namespace

ChromeMainDelegate::ChromeMainDelegate() {
}

ChromeMainDelegate::~ChromeMainDelegate() {
}

bool ChromeMainDelegate::BasicStartupComplete(int* exit_code) {
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  std::string process_type =
    command_line.GetSwitchValueASCII(switches::kProcessType);

  content::SetContentClient(&chrome_content_client_);

  // Android does InitLogging when library is loaded. Skip here.
  logging::OldFileDeletionState file_state = logging::APPEND_TO_OLD_LOG_FILE;
  if (process_type.empty()) {
    file_state = logging::DELETE_OLD_LOG_FILE;
  }
  logging::InitChromeLogging(command_line, file_state);
  LOG(INFO) << "logging::InitChromeLogging";

  return false;
}

int ChromeMainDelegate::RunProcess(
    const std::string& process_type,
    const content::MainFunctionParams& main_function_params) {

  return -1;
}

void ChromeMainDelegate::ProcessExiting(const std::string& process_type) {
  LOG(INFO) << "logging::CleanupChromeLogging";
  logging::CleanupChromeLogging();
}

content::ContentBrowserClient*
ChromeMainDelegate::CreateContentBrowserClient() {
  return g_chrome_content_browser_client.Pointer();
}
