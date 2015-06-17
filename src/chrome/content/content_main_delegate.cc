// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/content/content_main_delegate.h"
#include "chrome/content/content_browser_client.h"

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
