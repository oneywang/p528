// Copyright by wangtianping. All rights reserved.

#ifndef CONTENT_PUBLIC_BROWSER_BROWSER_MAIN_RUNNER_H_
#define CONTENT_PUBLIC_BROWSER_BROWSER_MAIN_RUNNER_H_

#include "base/basictypes.h"
#include "content/content_export.h"

namespace content {

struct MainFunctionParams;

// This class is responsible for browser initialization, running and shutdown.
class CONTENT_EXPORT BrowserMainRunner {
 public:
  virtual ~BrowserMainRunner() {}

  // Create a new BrowserMainRunner object.
  static BrowserMainRunner* Create();

  // Initialize all necessary browser state. The |parameters| values will be
  // copied. Returning a non-negative value indicates that initialization
  // failed, and the returned value is used as the exit code for the process.
  virtual int Initialize(const content::MainFunctionParams& parameters) = 0;

  // Perform the default run logic.
  virtual int Run() = 0;

  // Shut down the browser state.
  virtual void Shutdown() = 0;
};

}  // namespace content

#endif  // CONTENT_PUBLIC_BROWSER_BROWSER_MAIN_RUNNER_H_
