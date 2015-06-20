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

#include <QString>
#include <QApplication>
#include <QDialog>
#include <QLabel>

using content::BrowserThread;

// BrowserMainParts ------------------------------------------------------------

ChromeBrowserMainParts::ChromeBrowserMainParts(
    const content::MainFunctionParams& parameters)
    : parameters_(parameters),
      parsed_command_line_(parameters.command_line),
      result_code_(content::RESULT_CODE_NORMAL_EXIT),
      run_message_loop_(true){
}

ChromeBrowserMainParts::~ChromeBrowserMainParts() {
}

// -----------------------------------------------------------------------------
// TODO(viettrungluu): move more/rest of BrowserMain() into BrowserMainParts.

//#if defined(OS_WIN)
//#define DLLEXPORT __declspec(dllexport)

// We use extern C for the prototype DLLEXPORT to avoid C++ name mangling.
//extern "C" {
//DLLEXPORT void __cdecl RelaunchChromeBrowserWithNewCommandLineIfNeeded();
//}

//DLLEXPORT void __cdecl RelaunchChromeBrowserWithNewCommandLineIfNeeded() {
  // Need an instance of AtExitManager to handle singleton creations and
  // deletions.  We need this new instance because, the old instance created
  // in ChromeMain() got destructed when the function returned.
  //base::AtExitManager exit_manager;
//}
//#endif

// content::BrowserMainParts implementation ------------------------------------

void ChromeBrowserMainParts::PreEarlyInitialization() {
}

void ChromeBrowserMainParts::PostEarlyInitialization() {
}

void ChromeBrowserMainParts::ToolkitInitialized() {
}

void ChromeBrowserMainParts::PreMainMessageLoopStart() {
}

void ChromeBrowserMainParts::PostMainMessageLoopStart() {
}

int ChromeBrowserMainParts::PreCreateThreads() {
  result_code_ = PreCreateThreadsImpl();

  if (result_code_ == content::RESULT_CODE_NORMAL_EXIT) {
  }

  return result_code_;
}

int ChromeBrowserMainParts::PreCreateThreadsImpl() {
  run_message_loop_ = false;

  {
    TRACE_EVENT0("startup",
      "ChromeBrowserMainParts::PreCreateThreadsImpl:InitBrowswerProcessImpl");
    browser_process_.reset(new BrowserProcessImpl(nullptr,
                                                  parsed_command_line()));
  }

  browser_process_->PreCreateThreads();

  return content::RESULT_CODE_NORMAL_EXIT;
}

void ChromeBrowserMainParts::PreMainMessageLoopRun() {
  result_code_ = PreMainMessageLoopRunImpl();

}

// PreMainMessageLoopRun calls these extra stages in the following order:
//  PreMainMessageLoopRunImpl()
//   ... initial setup, including browser_process_ setup.
//   PreProfileInit()
//   ... additional setup, including CreateProfile()
//   PostProfileInit()
//   ... additional setup
//   PreBrowserStart()
//   ... browser_creator_->Start
//   PostBrowserStart()

void ChromeBrowserMainParts::PreProfileInit() {

}

void ChromeBrowserMainParts::PostProfileInit() {
}

void ChromeBrowserMainParts::PreBrowserStart() {
}

void ChromeBrowserMainParts::PostBrowserStart() {
}

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

void ChromeBrowserMainParts::TestBreakpad(){
  //ok
  //DerefZeroCrash();
  
  //ok<-__debugbreak
  //InvalidParamCrash();

  //ok<-__debreakbreak
  //PureCallCrash();

  //ok
  //base::debug::DumpWithoutCrashing();

  //ok
  QString str = QString::fromWCharArray(L"HelloQt");
  LOG(INFO) << str.toStdString();
  
  //int argc = 0;
  //char** argv = NULL;
  //(new QCoreApplication(argc, argv))->exec();
}

int ChromeBrowserMainParts::PreMainMessageLoopRunImpl() {
  browser_process_->PreMainMessageLoopRun();

  PreProfileInit();

  PostProfileInit();

  PreBrowserStart();

  run_message_loop_ = true;

  // test,quit 5s later
  base::MessageLoop::current()->PostDelayedTask(
    FROM_HERE,
    base::Bind(&ChromeBrowserMainParts::TestBreakpad,base::Unretained(this)),
    base::TimeDelta::FromSeconds(4));

  base::MessageLoop::current()->PostDelayedTask(
    FROM_HERE,
    base::MessageLoop::QuitClosure(),
    base::TimeDelta::FromSeconds(5));

  PostBrowserStart();

  return result_code_;
}

bool ChromeBrowserMainParts::MainMessageLoopRun(int* result_code) {
  // Set the result code set in PreMainMessageLoopRun or set above.
  *result_code = result_code_;
  if (!run_message_loop_)
    return true;  // Don't run the default message loop.

  DCHECK(base::MessageLoopForUI::IsCurrent());
  base::RunLoop run_loop;

  //run_loop.Run();

  int argc = 0;
  char** argv = NULL;
  QApplication a(argc, argv);

  QDialog *dlg = new QDialog();
  QLabel *label = new QLabel(dlg);
  label->setText("HelloQt");
  dlg->show();
  a.exec();

  return true;
}

void ChromeBrowserMainParts::PostMainMessageLoopRun() {
  browser_process_->StartTearDown();
}

void ChromeBrowserMainParts::PostDestroyThreads() {
  browser_process_->PostDestroyThreads();
  // browser_shutdown takes care of deleting browser_process, so we need to
  // release it.
  ignore_result(browser_process_.release());
}
