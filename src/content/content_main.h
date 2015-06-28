// Copyright by wangtianping. All rights reserved.

#ifndef CONTENT_PUBLIC_APP_CONTENT_MAIN_H_
#define CONTENT_PUBLIC_APP_CONTENT_MAIN_H_

#include <stddef.h>
#include <windows.h>

#include "base/callback_forward.h"
#include "build/build_config.h"
#include "content/content_export.h"

namespace content {
class ContentMainDelegate;

struct ContentMainParams {
  explicit ContentMainParams(ContentMainDelegate* delegate)
      : delegate(delegate),
        instance(NULL){
  }

  ContentMainDelegate* delegate;
  HINSTANCE instance;
};

// ContentMain should be called from the embedder's main() function to do the
// initial setup for every process. The embedder has a chance to customize
// startup using the ContentMainDelegate interface. The embedder can also pass
// in NULL for |delegate| if they don't want to override default startup.
CONTENT_EXPORT int ContentMain(const ContentMainParams& params);

}  // namespace content

#endif  // CONTENT_PUBLIC_APP_CONTENT_MAIN_H_
