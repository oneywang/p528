// Copyright by wangtianping. All rights reserved.

#include "content/browser_main.h"

#include "base/trace_event/trace_event.h"
#include "content/browser_main_runner.h"

namespace content {

// Main routine for running as the Browser process.
int BrowserMain(const MainFunctionParams& parameters) { 
  scoped_ptr<BrowserMainRunner> main_runner(BrowserMainRunner::Create());

  int exit_code = main_runner->Initialize(parameters);
  if (exit_code >= 0)
    return exit_code;

  exit_code = main_runner->Run();

  main_runner->Shutdown();

  return exit_code;
}

}  // namespace content
