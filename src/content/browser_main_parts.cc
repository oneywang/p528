// Copyright by wangtianping. All rights reserved.

#include "content/browser_main_parts.h"

namespace content {

int BrowserMainParts::PreCreateThreads() {
  return 0;
}

bool BrowserMainParts::MainMessageLoopRun(int* result_code) {
  return false;
}

}  // namespace content
