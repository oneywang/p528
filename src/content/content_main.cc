// Copyright by wangtianping. All rights reserved.

#include "content/content_main.h"

#include "base/memory/scoped_ptr.h"
#include "content/content_main_runner.h"

namespace content {

int ContentMain(const ContentMainParams& params) {
  scoped_ptr<ContentMainRunner> main_runner(ContentMainRunner::Create());

  int exit_code = main_runner->Initialize(params);
  if (exit_code >= 0)
    return exit_code;

  exit_code = main_runner->Run();

  main_runner->Shutdown();

  return exit_code;
}

}  // namespace content
