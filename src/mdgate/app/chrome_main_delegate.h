// Copyright by wangtianping. All rights reserved.

#ifndef CHROME_APP_CHROME_MAIN_DELEGATE_H_
#define CHROME_APP_CHROME_MAIN_DELEGATE_H_

#include "base/memory/scoped_ptr.h"
#include "content/content_main_delegate.h"
#include "mdgate/browser/chrome_content_client.h"

template <typename>
class ScopedVector;

namespace base {
class CommandLine;
}

// Chrome implementation of ContentMainDelegate.
class ChromeMainDelegate : public content::ContentMainDelegate {
 public:
  ChromeMainDelegate();
  ~ChromeMainDelegate() override;

 protected:
  // content::ContentMainDelegate implementation:
  bool BasicStartupComplete(int* exit_code) override;
  int RunProcess(
      const std::string& process_type,
      const content::MainFunctionParams& main_function_params) override;
  void ProcessExiting(const std::string& process_type) override;
  content::ContentBrowserClient* CreateContentBrowserClient() override;

  ChromeContentClient chrome_content_client_;

  DISALLOW_COPY_AND_ASSIGN(ChromeMainDelegate);
};

#endif  // CHROME_APP_CHROME_MAIN_DELEGATE_H_
