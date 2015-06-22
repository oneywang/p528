// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chrome_browser_main.h"

#include <set>
#include <string>
#include <vector>

#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/debug/crash_logging.h"
#include "base/debug/debugger.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/path_service.h"

#include "base/process/process_info.h"
#include "base/profiler/scoped_tracker.h"
#include "base/run_loop.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_split.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/sys_info.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "base/values.h"
#include "build/build_config.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/browser_process_impl.h"

#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_result_codes.h"
//#include "chrome/common/chrome_switches.h"
#include "chrome/common/env_vars.h"

#include "chrome/content/browser_thread.h"

#include "chrome/content/content_client.h"
#include "chrome/common/content_switches.h"
#include "chrome/content/main_function_params.h"

#include "chrome/browser/abstract_class.h"
#include "base/debug/dump_without_crashing.h"

#include "chrome/browser/messageloop_qt.h"

#include <QtCore/QString>
#include <QtWidgets/QPushButton>
#include <QApplication>

using content::BrowserThread;

// BrowserMainParts ------------------------------------------------------------

ChromeBrowserMainParts::ChromeBrowserMainParts(
    const content::MainFunctionParams& parameters)
    : parameters_(parameters),
      parsed_command_line_(parameters.command_line),
      result_code_(content::RESULT_CODE_NORMAL_EXIT){
}

ChromeBrowserMainParts::~ChromeBrowserMainParts() {
}

// content::BrowserMainParts implementation ------------------------------------

void ChromeBrowserMainParts::PreEarlyInitialization() {
}

void ChromeBrowserMainParts::PostEarlyInitialization() {
}

void ChromeBrowserMainParts::ToolkitInitialized() {
  QtInit();
}

void ChromeBrowserMainParts::PreMainMessageLoopStart() {
  base::MessageLoop::InitMessagePumpForUIFactory(QtMessagePumpFactory);
}

void ChromeBrowserMainParts::PostMainMessageLoopStart() {
}

int ChromeBrowserMainParts::PreCreateThreads() {
  browser_process_.reset(new BrowserProcessImpl(nullptr,parsed_command_line()));
  browser_process_->PreCreateThreads();

  result_code_ =  content::RESULT_CODE_NORMAL_EXIT;
  return result_code_;
}

void ChromeBrowserMainParts::PreMainMessageLoopRun() {
  browser_process_->PreMainMessageLoopRun();

  // unittest
  base::MessageLoop::current()->PostDelayedTask(
    FROM_HERE,
    base::Bind(&ChromeBrowserMainParts::UnitTest, base::Unretained(this)),
    base::TimeDelta::FromSeconds(5));

  // create mainwindow&show
  QPushButton *button = new QPushButton("quit");
  QObject::connect(button, &QPushButton::clicked, QtApp(), &QApplication::quit);
  button->show();
}

bool ChromeBrowserMainParts::MainMessageLoopRun(int* result_code) {
  // Set the result code set in PreMainMessageLoopRun or set above.
  *result_code = result_code_;


  //DCHECK(base::MessageLoopForUI::IsCurrent());
  //base::RunLoop run_loop;
  //run_loop.Run();
  //run_message_loop_ = true;

#if 0
  QtApp()->exec();
  return true;
#else
  return false;
#endif
}

void ChromeBrowserMainParts::PostMainMessageLoopRun() {
  browser_process_->StartTearDown();
}

void ChromeBrowserMainParts::PostDestroyThreads() {
  browser_process_->PostDestroyThreads();
  ignore_result(browser_process_.release());
  delete g_browser_process;
  g_browser_process = nullptr;

  QtFini();
}

//////////////////////////////////////////////////////////////////////////

void ChromeBrowserMainParts::DerefZeroCrash() {
  int* x = 0;
  *x = 1;
  base::debug::Alias(x);
}

void ChromeBrowserMainParts::InvalidParamCrash() {
  printf(NULL);
}

void ChromeBrowserMainParts::PureCallCrash() {
  google_breakpad::Derived derived;
  base::debug::Alias(&derived);
}

void ChromeBrowserMainParts::UnitTest(){
  //ok
  //DerefZeroCrash();

  //ok<-__debugbreak
  //InvalidParamCrash();

  //ok<-__debreakbreak
  //PureCallCrash();

  //ok
  //base::debug::DumpWithoutCrashing();

  //ok
  //QString str = QString::fromWCharArray(L"HelloQt");
  //LOG(INFO) << str.toStdString();

  //ok
  //base::MessageLoop::current()->QuitNow();

  //ok,for aboutToBlock
  base::MessageLoop::current()->QuitWhenIdle();
}

//////////////////////////////////////////////////////////////////////////
