// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/content/content_client.h"

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
