// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/app/chrome_main_delegate.h"

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
#include "chrome/browser/chrome_content_browser_client.h"
#include "chrome/common/content_switches.h"

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

  //chrome::RegisterPathProvider();

  content::SetContentClient(&chrome_content_client_);

  // Initialize the user data dir for any process type that needs it.
  //if (chrome::ProcessNeedsProfileDir(process_type))
  //  InitializeUserDataDir();

  // Android does InitLogging when library is loaded. Skip here.
  //logging::OldFileDeletionState file_state =
  //  logging::APPEND_TO_OLD_LOG_FILE;
  //if (process_type.empty()) {
  //  file_state = logging::DELETE_OLD_LOG_FILE;
  //}
  //logging::InitChromeLogging(command_line, file_state);

  return false;
}

int ChromeMainDelegate::RunProcess(
    const std::string& process_type,
    const content::MainFunctionParams& main_function_params) {

  return -1;
}

void ChromeMainDelegate::ProcessExiting(const std::string& process_type) {
  //logging::CleanupChromeLogging();
}

content::ContentBrowserClient*
ChromeMainDelegate::CreateContentBrowserClient() {
  return g_chrome_content_browser_client.Pointer();
}

