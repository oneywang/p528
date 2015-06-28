// Copyright by wangtianping. All rights reserved.

#ifndef CONTENT_PUBLIC_COMMON_CONTENT_CLIENT_H_
#define CONTENT_PUBLIC_COMMON_CONTENT_CLIENT_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/strings/string16.h"
#include "base/strings/string_piece.h"
#include "build/build_config.h"
#include "content/content_export.h"

class GURL;

namespace base {
class RefCountedStaticMemory;
}

namespace content {

class ContentBrowserClient;
class ContentClient;

// Setter and getter for the client.  The client should be set early, before any
// content code is called.
CONTENT_EXPORT void SetContentClient(ContentClient* client);

// Content's embedder API should only be used by content.
ContentClient* GetContentClient();


// Interface that the embedder implements.
class CONTENT_EXPORT ContentClient {
 public:
  ContentClient();
  virtual ~ContentClient();

  ContentBrowserClient* browser() { return browser_; }

 private:
  friend class ContentClientInitializer;  // To set these pointers.

  // The embedder API for participating in browser logic.
  ContentBrowserClient* browser_;
};

}  // namespace content

#endif  // CONTENT_PUBLIC_COMMON_CONTENT_CLIENT_H_
