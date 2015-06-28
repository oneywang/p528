// Copyright by wangtianping. All rights reserved.

#include "mdgate/browser/chrome_content_browser_client.h"

#include "mdgate/browser/chrome_browser_main.h"

namespace chrome {

ChromeContentBrowserClient::ChromeContentBrowserClient()
    :weak_factory_(this) {
}

ChromeContentBrowserClient::~ChromeContentBrowserClient() {
}

content::BrowserMainParts* ChromeContentBrowserClient::CreateBrowserMainParts(
    const content::MainFunctionParams& parameters) {
  ChromeBrowserMainParts* main_parts;
  main_parts = new ChromeBrowserMainParts(parameters);
  return main_parts;
}

}  // namespace chrome
