// Copyright by wangtianping. All rights reserved.

#include "content/content_client.h"

#include "base/logging.h"
#include "base/strings/string_piece.h"

namespace content {

static ContentClient* g_client;

void SetContentClient(ContentClient* client) {
  g_client = client;
}

ContentClient* GetContentClient() {
  return g_client;
}

ContentClient::ContentClient()
    : browser_(NULL){
}

ContentClient::~ContentClient() {
}


}  // namespace content
