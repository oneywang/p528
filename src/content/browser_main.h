// Copyright by wangtianping. All rights reserved.

#ifndef CONTENT_BROWSER_BROWSER_MAIN_H_
#define CONTENT_BROWSER_BROWSER_MAIN_H_

#include "base/basictypes.h"
#include "content/content_export.h"

namespace content {

struct MainFunctionParams;

bool ExitedMainMessageLoop();

CONTENT_EXPORT int BrowserMain(const content::MainFunctionParams& parameters);

}  // namespace content

#endif  // CONTENT_BROWSER_BROWSER_MAIN_H_
