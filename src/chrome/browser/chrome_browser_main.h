// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROME_BROWSER_MAIN_H_
#define CHROME_BROWSER_CHROME_BROWSER_MAIN_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/metrics/field_trial.h"
#include "base/tracked_objects.h"
#include "chrome/content/browser_main_parts.h"
#include "chrome/content/main_function_params.h"

class QApplication;
class BrowserProcessImpl;

class ChromeBrowserMainParts : public content::BrowserMainParts {
 public:
  ~ChromeBrowserMainParts() override;

  explicit ChromeBrowserMainParts(
      const content::MainFunctionParams& parameters);

  // content::BrowserMainParts overrides.
  // These are called in-order by content::BrowserMainLoop.
  // Each stage calls the same stages in any ChromeBrowserMainExtraParts added
  // with AddParts() from ChromeContentBrowserClient::CreateBrowserMainParts.
  void PreEarlyInitialization() override;
  void PostEarlyInitialization() override;
  void ToolkitInitialized() override;
  void PreMainMessageLoopStart() override;
  void PostMainMessageLoopStart() override;
  int PreCreateThreads() override;
  void PreMainMessageLoopRun() override;
  bool MainMessageLoopRun(int* result_code) override;
  void PostMainMessageLoopRun() override;
  void PostDestroyThreads() override;

  const content::MainFunctionParams& parameters() const {
    return parameters_;
  }
  const base::CommandLine& parsed_command_line() const {
    return parsed_command_line_;
  }

 private:
   void UnitTest();
   void DerefZeroCrash();
   void InvalidParamCrash();
   void PureCallCrash();

 private:
  // Members initialized on construction ---------------------------------------

  const content::MainFunctionParams parameters_;
  const base::CommandLine& parsed_command_line_;
  int result_code_;

  // Members initialized after / released before main_message_loop_ ------------

  scoped_ptr<BrowserProcessImpl> browser_process_;

  DISALLOW_COPY_AND_ASSIGN(ChromeBrowserMainParts);
};

#endif  // CHROME_BROWSER_CHROME_BROWSER_MAIN_H_
