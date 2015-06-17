// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/content/browser_main.h"

#include "base/trace_event/trace_event.h"
#include "chrome/content/browser_main_runner.h"

namespace content {

// Main routine for running as the Browser process.
//TODO(hege):MainFunctionParams->BrowserMainParts
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
