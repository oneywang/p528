// Copyright by wangtianping. All rights reserved.

#include "content/browser_main_runner.h"

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/debug/leak_annotations.h"
#include "base/logging.h"
#include "base/win/win_util.h"
#include "base/win/windows_version.h"
#include "content/browser_main_loop.h"
#include "content/content_switches.h"
#include "content/main_function_params.h"

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
      g_exited_main_message_loop = true;

      main_loop_->ShutdownThreadsAndCleanUp();

      main_loop_.reset(NULL);

      is_shutdown_ = true;
    }
  }

 protected:
  // True if we have started to initialize the runner.
  bool initialization_started_;

  // True if the runner has been shut down.
  bool is_shutdown_;

  scoped_ptr<BrowserMainLoop> main_loop_;

  DISALLOW_COPY_AND_ASSIGN(BrowserMainRunnerImpl);
};

// static
BrowserMainRunner* BrowserMainRunner::Create() {
  return new BrowserMainRunnerImpl();
}

}  // namespace content
