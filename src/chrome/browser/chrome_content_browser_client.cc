// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chrome_content_browser_client.h"

#include <set>
#include <utility>
#include <vector>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/scoped_file.h"
#include "base/i18n/icu_util.h"
#include "base/lazy_instance.h"
#include "base/path_service.h"
#include "base/prefs/pref_service.h"
#include "base/prefs/scoped_user_pref_update.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/sequenced_worker_pool.h"

#include "chrome/browser/chrome_browser_main.h"
namespace chrome {

ChromeContentBrowserClient::ChromeContentBrowserClient()
    :weak_factory_(this) {
}

ChromeContentBrowserClient::~ChromeContentBrowserClient() {
}

content::BrowserMainParts* ChromeContentBrowserClient::CreateBrowserMainParts(
    const content::MainFunctionParams& parameters) {
  ChromeBrowserMainParts* main_parts;
  main_parts = new ChromeBrowserMainParts(parameters);
  return main_parts;
}

}  // namespace chrome
