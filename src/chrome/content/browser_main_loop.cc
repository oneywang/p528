// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/content/browser_main_loop.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/metrics/field_trial.h"
#include "base/metrics/histogram.h"
#include "base/pending_task.h"
#include "base/power_monitor/power_monitor.h"
#include "base/power_monitor/power_monitor_device_source.h"
#include "base/process/process_metrics.h"
#include "base/run_loop.h"
#include "base/strings/string_number_conversions.h"
#include "base/system_monitor/system_monitor.h"
#include "base/thread_task_runner_handle.h"
#include "base/threading/thread_restrictions.h"
#include "base/timer/hi_res_timer_manager.h"
#include "base/trace_event/trace_event.h"
#include "chrome/content/browser_thread_impl.h"
#include "chrome/content/startup_task_runner.h"
#include "chrome/content/browser_main_parts.h"
#include "chrome/content/browser_shutdown.h"
#include "chrome/content/content_client.h"
#include "chrome/content/content_browser_client.h"
#include "chrome/content/main_function_params.h"
#include "chrome/common/content_switches.h"
#include "chrome/common/result_codes.h"

#if defined(OS_WIN)
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>

//#include "content/browser/system_message_window_win.h"
//#include "content/common/sandbox_win.h"
//#include "net/base/winsock_init.h"
//#include "ui/base/l10n/l10n_util_win.h"
#endif

namespace content {
namespace {

// Disable optimizations for this block of functions so the compiler doesn't
// merge them all together. This makes it possible to tell what thread was
// unresponsive by inspecting the callstack.
MSVC_DISABLE_OPTIMIZE()
MSVC_PUSH_DISABLE_WARNING(4748)

NOINLINE void ResetThread_DB(scoped_ptr<BrowserProcessSubThread> thread) {
  volatile int inhibit_comdat = __LINE__;
  ALLOW_UNUSED_LOCAL(inhibit_comdat);
  thread.reset();
}

NOINLINE void ResetThread_FILE(scoped_ptr<BrowserProcessSubThread> thread) {
  volatile int inhibit_comdat = __LINE__;
  ALLOW_UNUSED_LOCAL(inhibit_comdat);
  thread.reset();
}

NOINLINE void ResetThread_FILE_USER_BLOCKING(
    scoped_ptr<BrowserProcessSubThread> thread) {
  volatile int inhibit_comdat = __LINE__;
  ALLOW_UNUSED_LOCAL(inhibit_comdat);
  thread.reset();
}

NOINLINE void ResetThread_PROCESS_LAUNCHER(
    scoped_ptr<BrowserProcessSubThread> thread) {
  volatile int inhibit_comdat = __LINE__;
  ALLOW_UNUSED_LOCAL(inhibit_comdat);
  thread.reset();
}

NOINLINE void ResetThread_CACHE(scoped_ptr<BrowserProcessSubThread> thread) {
  volatile int inhibit_comdat = __LINE__;
  ALLOW_UNUSED_LOCAL(inhibit_comdat);
  thread.reset();
}

NOINLINE void ResetThread_IO(scoped_ptr<BrowserProcessSubThread> thread) {
  volatile int inhibit_comdat = __LINE__;
  ALLOW_UNUSED_LOCAL(inhibit_comdat);
  thread.reset();
}

MSVC_POP_WARNING()
MSVC_ENABLE_OPTIMIZE();

}  // namespace

// The currently-running BrowserMainLoop.  There can be one or zero.
BrowserMainLoop* g_current_browser_main_loop = NULL;

// This is just to be able to keep ShutdownThreadsAndCleanUp out of
// the public interface of BrowserMainLoop.
class BrowserShutdownImpl {
 public:
  static void ImmediateShutdownAndExitProcess() {
    DCHECK(g_current_browser_main_loop);
    g_current_browser_main_loop->ShutdownThreadsAndCleanUp();

#if defined(OS_WIN)
    // At this point the message loop is still running yet we've shut everything
    // down. If any messages are processed we'll likely crash. Exit now.
    ExitProcess(RESULT_CODE_NORMAL_EXIT);
#elif defined(OS_POSIX) && !defined(OS_MACOSX)
    _exit(RESULT_CODE_NORMAL_EXIT);
#else
    NOTIMPLEMENTED();
#endif
  }
};

void ImmediateShutdownAndExitProcess() {
  BrowserShutdownImpl::ImmediateShutdownAndExitProcess();
}

// BrowserMainLoop construction / destruction =============================

BrowserMainLoop* BrowserMainLoop::GetInstance() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  return g_current_browser_main_loop;
}

BrowserMainLoop::BrowserMainLoop(const MainFunctionParams& parameters)
    : parameters_(parameters),
      parsed_command_line_(parameters.command_line),
      result_code_(RESULT_CODE_NORMAL_EXIT){
  DCHECK(!g_current_browser_main_loop);
  g_current_browser_main_loop = this;
}

BrowserMainLoop::~BrowserMainLoop() {
  DCHECK_EQ(this, g_current_browser_main_loop);
  g_current_browser_main_loop = NULL;
}

void BrowserMainLoop::Init() {
  parts_.reset(
      GetContentClient()->browser()->CreateBrowserMainParts(parameters_));
}

// BrowserMainLoop stages ==================================================

void BrowserMainLoop::EarlyInitialization() {
  if (parts_)
    parts_->PreEarlyInitialization();

  //do something....

  if (parts_)
    parts_->PostEarlyInitialization();
}

void BrowserMainLoop::MainMessageLoopStart() {
  if (parts_) {
    parts_->PreMainMessageLoopStart();
  }

  // Create a MessageLoop if one does not already exist for the current thread.
  if (!base::MessageLoop::current())
    main_message_loop_.reset(new base::MessageLoopForUI);

  InitializeMainThread();

  {
    TRACE_EVENT0("startup", "BrowserMainLoop::Subsystem:SystemMonitor");
    //system_monitor_.reset(new base::SystemMonitor);
  }
  {
    TRACE_EVENT0("startup", "BrowserMainLoop::Subsystem:PowerMonitor");
    scoped_ptr<base::PowerMonitorSource> power_monitor_source(
      new base::PowerMonitorDeviceSource());
    power_monitor_.reset(new base::PowerMonitor(power_monitor_source.Pass()));
  }
  {
    TRACE_EVENT0("startup", "BrowserMainLoop::Subsystem:HighResTimerManager");
    hi_res_timer_manager_.reset(new base::HighResolutionTimerManager);
  }

#if defined(OS_WIN)
  //system_message_window_.reset(new SystemMessageWindowWin);
#endif

  if (parts_)
    parts_->PostMainMessageLoopStart();
}

int BrowserMainLoop::PreCreateThreads() {
  if (parts_) {
    result_code_ = parts_->PreCreateThreads();
  }

  return result_code_;
}

void BrowserMainLoop::CreateStartupTasks() {
  // First time through, we really want to create all the tasks
  if (!startup_task_runner_.get()) {
    startup_task_runner_ = make_scoped_ptr(new StartupTaskRunner(
        base::Callback<void(int)>(),
        base::MessageLoop::current()->message_loop_proxy()));

    StartupTask pre_create_threads =
        base::Bind(&BrowserMainLoop::PreCreateThreads, base::Unretained(this));
    startup_task_runner_->AddTask(pre_create_threads);

    StartupTask create_threads =
        base::Bind(&BrowserMainLoop::CreateThreads, base::Unretained(this));
    startup_task_runner_->AddTask(create_threads);

    StartupTask browser_thread_started = base::Bind(
        &BrowserMainLoop::BrowserThreadsStarted, base::Unretained(this));
    startup_task_runner_->AddTask(browser_thread_started);

    StartupTask pre_main_message_loop_run = base::Bind(
        &BrowserMainLoop::PreMainMessageLoopRun, base::Unretained(this));
    startup_task_runner_->AddTask(pre_main_message_loop_run);
  }

  startup_task_runner_->RunAllTasksNow();
}

int BrowserMainLoop::CreateThreads() {
  base::Thread::Options io_message_loop_options;
  io_message_loop_options.message_loop_type = base::MessageLoop::TYPE_IO;
  base::Thread::Options ui_message_loop_options;
  ui_message_loop_options.message_loop_type = base::MessageLoop::TYPE_UI;

  // Start threads in the order they occur in the BrowserThread::ID
  // enumeration, except for BrowserThread::UI which is the main
  // thread.
  //
  // Must be size_t so we can increment it.
  for (size_t thread_id = BrowserThread::UI + 1;
       thread_id < BrowserThread::ID_COUNT;
       ++thread_id) {
    scoped_ptr<BrowserProcessSubThread>* thread_to_start = NULL;
    base::Thread::Options options;

    switch (thread_id) {
      case BrowserThread::DB:
        thread_to_start = &db_thread_;
        options.timer_slack = base::TIMER_SLACK_MAXIMUM;
        break;
      case BrowserThread::FILE_USER_BLOCKING:
        thread_to_start = &file_user_blocking_thread_;
        break;
      case BrowserThread::FILE:
        thread_to_start = &file_thread_;
//#if defined(OS_WIN)
        // On Windows, the FILE thread needs to be have a UI message loop
        // which pumps messages in such a way that Google Update can
        // communicate back to us.
//        options = ui_message_loop_options;
//#else
        options = io_message_loop_options;
//#endif
        options.timer_slack = base::TIMER_SLACK_MAXIMUM;
        break;
      case BrowserThread::PROCESS_LAUNCHER:
        thread_to_start = &process_launcher_thread_;
        options.timer_slack = base::TIMER_SLACK_MAXIMUM;
        break;
      case BrowserThread::CACHE:
        thread_to_start = &cache_thread_;
        options = io_message_loop_options;
        options.timer_slack = base::TIMER_SLACK_MAXIMUM;
        break;
      case BrowserThread::IO:
        thread_to_start = &io_thread_;
        options = io_message_loop_options;
        break;
      case BrowserThread::UI:
      case BrowserThread::ID_COUNT:
      default:
        NOTREACHED();
        break;
    }

    BrowserThread::ID id = static_cast<BrowserThread::ID>(thread_id);

    if (thread_to_start) {
      (*thread_to_start).reset(new BrowserProcessSubThread(id));
      if (!(*thread_to_start)->StartWithOptions(options)) {
        LOG(FATAL) << "Failed to start the browser thread: id == " << id;
      }
    } else {
      NOTREACHED();
    }
  }
  created_threads_ = true;
  return result_code_;
}

int BrowserMainLoop::PreMainMessageLoopRun() {
  if (parts_) {
    parts_->PreMainMessageLoopRun();
  }

  // If the UI thread blocks, the whole UI is unresponsive.
  // Do not allow disk IO from the UI thread.
  base::ThreadRestrictions::SetIOAllowed(false);
  base::ThreadRestrictions::DisallowWaiting();
  return result_code_;
}

void BrowserMainLoop::RunMainMessageLoopParts() {
  bool ran_main_loop = false;
  if (parts_)
    ran_main_loop = parts_->MainMessageLoopRun(&result_code_);

  if (!ran_main_loop)
    MainMessageLoopRun();
}

void BrowserMainLoop::ShutdownThreadsAndCleanUp() {
  if (!created_threads_) {
    // Called early, nothing to do
    return;
  }

  // Teardown may start in PostMainMessageLoopRun, and during teardown we
  // need to be able to perform IO.
  base::ThreadRestrictions::SetIOAllowed(true);
  BrowserThread::PostTask(
      BrowserThread::IO, FROM_HERE,
      base::Bind(base::IgnoreResult(&base::ThreadRestrictions::SetIOAllowed),
                 true));

  if (parts_) {
    parts_->PostMainMessageLoopRun();
  }

  // The device monitors are using |system_monitor_| as dependency, so delete
  // them before |system_monitor_| goes away.
  // On Mac and windows, the monitor needs to be destroyed on the same thread
  // as they were created. On Linux, the monitor will be deleted when IO thread
  // goes away.
#if defined(OS_WIN)
  //system_message_window_.reset();
#endif  // !defined(OS_IOS)

  // Must be size_t so we can subtract from it.
  for (size_t thread_id = BrowserThread::ID_COUNT - 1;
       thread_id >= (BrowserThread::UI + 1);
       --thread_id) {
    // Find the thread object we want to stop. Looping over all valid
    // BrowserThread IDs and DCHECKing on a missing case in the switch
    // statement helps avoid a mismatch between this code and the
    // BrowserThread::ID enumeration.
    //
    // The destruction order is the reverse order of occurrence in the
    // BrowserThread::ID list. The rationale for the order is as
    // follows (need to be filled in a bit):
    //
    //
    // - The IO thread is the only user of the CACHE thread.
    //
    // - The PROCESS_LAUNCHER thread must be stopped after IO in case
    //   the IO thread posted a task to terminate a process on the
    //   process launcher thread.
    //
    // - (Not sure why DB stops last.)
    switch (thread_id) {
      case BrowserThread::DB: {
        ResetThread_DB(db_thread_.Pass());
        break;
      }
      case BrowserThread::FILE: {
        ResetThread_FILE(file_thread_.Pass());
        break;
      }
      case BrowserThread::FILE_USER_BLOCKING: {
        ResetThread_FILE_USER_BLOCKING(file_user_blocking_thread_.Pass());
        break;
      }
      case BrowserThread::PROCESS_LAUNCHER: {
        ResetThread_PROCESS_LAUNCHER(process_launcher_thread_.Pass());
        break;
      }
      case BrowserThread::CACHE: {
        ResetThread_CACHE(cache_thread_.Pass());
        break;
      }
      case BrowserThread::IO: {
        ResetThread_IO(io_thread_.Pass());
        break;
      }
      case BrowserThread::UI:
      case BrowserThread::ID_COUNT:
      default:
        NOTREACHED();
        break;
    }
  }

  // Close the blocking I/O pool after the other threads. Other threads such
  // as the I/O thread may need to schedule work like closing files or flushing
  // data during shutdown, so the blocking pool needs to be available. There
  // may also be slow operations pending that will blcok shutdown, so closing
  // it here (which will block until required operations are complete) gives
  // more head start for those operations to finish.
  {
    BrowserThreadImpl::ShutdownThreadPool();
  }

  if (parts_) {
    parts_->PostDestroyThreads();
  }
}

void BrowserMainLoop::InitializeMainThread() {
  const char* kThreadName = "CrBrowserMain";
  base::PlatformThread::SetName(kThreadName);
  if (main_message_loop_)
    main_message_loop_->set_thread_name(kThreadName);

  // Register the main thread by instantiating it, but don't call any methods.
  main_thread_.reset(
      new BrowserThreadImpl(BrowserThread::UI, base::MessageLoop::current()));
}

int BrowserMainLoop::BrowserThreadsStarted() {
  return result_code_;
}

bool BrowserMainLoop::InitializeToolkit() {
#if defined(OS_WIN)
  // Init common control sex.
  INITCOMMONCONTROLSEX config;
  config.dwSize = sizeof(config);
  config.dwICC = ICC_WIN95_CLASSES;
  if (!InitCommonControlsEx(&config))
    PLOG(FATAL);
#endif

  if (parts_)
    parts_->ToolkitInitialized();

  return true;
}

void BrowserMainLoop::MainMessageLoopRun() {
#if defined(OS_ANDROID)
  // Android's main message loop is the Java message loop.
  NOTREACHED();
#else
  DCHECK(base::MessageLoopForUI::IsCurrent());

  base::RunLoop run_loop;
  run_loop.Run();
#endif
}

}  // namespace content
