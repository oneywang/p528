// Copyright by wangtianping. All rights reserved.
#include "content/content_main_delegate.h"

#include "content/content_browser_client.h"

namespace content {

bool ContentMainDelegate::BasicStartupComplete(int* exit_code) {
  return false;
}


ContentBrowserClient* ContentMainDelegate::CreateContentBrowserClient() {
  return new ContentBrowserClient();
}

int ContentMainDelegate::RunProcess(
  const std::string& process_type,
  const content::MainFunctionParams& main_function_params) {
  return -1;
}

}  // namespace content
