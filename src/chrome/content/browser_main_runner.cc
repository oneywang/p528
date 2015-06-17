// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/content/browser_main_runner.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/debug/leak_annotations.h"
#include "base/logging.h"
#include "base/metrics/histogram.h"
#include "base/metrics/statistics_recorder.h"
#include "base/profiler/scoped_tracker.h"
#include "base/trace_event/trace_event.h"
#include "base/tracked_objects.h"
#include "chrome/content/browser_main_loop.h"
//#include "content/browser/browser_shutdown_profile_dumper.h"
//#include "content/browser/notification_service_impl.h"
#include "chrome/common/content_switches.h"
#include "chrome/content/main_function_params.h"
//#include "ui/base/ime/input_method_initializer.h"

#if defined(OS_WIN)
#include "base/win/win_util.h"
#include "base/win/windows_version.h"
//#include "net/cert/sha256_legacy_support_win.h"
//#include "sandbox/win/src/sidestep/preamble_patcher.h"
//#include "ui/base/win/scoped_ole_initializer.h"
//#include "ui/gfx/switches.h"
//#include "ui/gfx/win/direct_write.h"
#endif

bool g_exited_main_message_loop = false;

namespace content {

class BrowserMainRunnerImpl : public BrowserMainRunner {
 public:
  BrowserMainRunnerImpl()
      : initialization_started_(false), is_shutdown_(false) {}

  ~BrowserMainRunnerImpl() override {
    if (initialization_started_ && !is_shutdown_)
      Shutdown();
  }

  int Initialize(const MainFunctionParams& parameters) override {
    // On Android we normally initialize the browser in a series of UI thread
    // tasks. While this is happening a second request can come from the OS or
    // another application to start the browser. If this happens then we must
    // not run these parts of initialization twice.
    if (!initialization_started_) {
      initialization_started_ = true;

      if (parameters.command_line.HasSwitch(switches::kWaitForDebugger))
        base::debug::WaitForDebugger(60, true);

#if defined(OS_WIN)
      if (base::win::GetVersion() < base::win::VERSION_VISTA) {
        // When "Extend support of advanced text services to all programs"
        // (a.k.a. Cicero Unaware Application Support; CUAS) is enabled on
        // Windows XP and handwriting modules shipped with Office 2003 are
        // installed, "penjpn.dll" and "skchui.dll" will be loaded and then
        // crash unless a user installs Office 2003 SP3. To prevent these
        // modules from being loaded, disable TSF entirely. crbug.com/160914.
        // TODO(yukawa): Add a high-level wrapper for this instead of calling
        // Win32 API here directly.
        ImmDisableTextFrameService(static_cast<DWORD>(-1));
      }
#endif  // OS_WIN

      base::StatisticsRecorder::Initialize();

      //notification_service_.reset(new NotificationServiceImpl);

#if defined(OS_WIN)
      // Ole must be initialized before starting message pump, so that TSF
      // (Text Services Framework) module can interact with the message pump
      // on Windows 8 Metro mode.
      //ole_initializer_.reset(new ui::ScopedOleInitializer);
      // Enable DirectWrite font rendering if needed.
      //gfx::win::MaybeInitializeDirectWrite();
#endif  // OS_WIN

      main_loop_.reset(new BrowserMainLoop(parameters));

      main_loop_->Init();

      main_loop_->EarlyInitialization();

      // Must happen before we try to use a message loop or display any UI.
      if (!main_loop_->InitializeToolkit())
        return 1;

      main_loop_->MainMessageLoopStart();

// WARNING: If we get a WM_ENDSESSION, objects created on the stack here
// are NOT deleted. If you need something to run during WM_ENDSESSION add it
// to browser_shutdown::Shutdown or BrowserProcess::EndSession.

      //ui::InitializeInputMethod();
    }
    main_loop_->CreateStartupTasks();
    int result_code = main_loop_->GetResultCode();
    if (result_code > 0)
      return result_code;

    // Return -1 to indicate no early termination.
    return -1;
  }

  int Run() override {
    DCHECK(initialization_started_);
    DCHECK(!is_shutdown_);
    main_loop_->RunMainMessageLoopParts();
    return main_loop_->GetResultCode();
  }

  void Shutdown() override {
    DCHECK(initialization_started_);
    DCHECK(!is_shutdown_);


    {
      // The trace event has to stay between profiler creation and destruction.
      TRACE_EVENT0("shutdown", "BrowserMainRunner");
      g_exited_main_message_loop = true;

      main_loop_->ShutdownThreadsAndCleanUp();

      //ui::ShutdownInputMethod();
  #if defined(OS_WIN)
      //ole_initializer_.reset(NULL);
  #endif
  #if defined(OS_ANDROID)
      // Forcefully terminates the RunLoop inside MessagePumpForUI, ensuring
      // proper shutdown for content_browsertests. Shutdown() is not used by
      // the actual browser.
      if (base::MessageLoop::current()->is_running())
        base::MessageLoop::current()->QuitNow();
  #endif
      main_loop_.reset(NULL);

      //notification_service_.reset(NULL);

      is_shutdown_ = true;
    }
  }

 protected:
  // True if we have started to initialize the runner.
  bool initialization_started_;

  // True if the runner has been shut down.
  bool is_shutdown_;

  //scoped_ptr<NotificationServiceImpl> notification_service_;
  scoped_ptr<BrowserMainLoop> main_loop_;
#if defined(OS_WIN)
  //scoped_ptr<ui::ScopedOleInitializer> ole_initializer_;
#endif

  DISALLOW_COPY_AND_ASSIGN(BrowserMainRunnerImpl);
};

// static
BrowserMainRunner* BrowserMainRunner::Create() {
  return new BrowserMainRunnerImpl();
}

}  // namespace content
