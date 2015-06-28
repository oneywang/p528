// Copyright by wangtianping. All rights reserved.

// When each service is created, we set a flag indicating this. At this point,
// the service initialization could fail or succeed. This allows us to remember
// if we tried to create a service, and not try creating it over and over if
// the creation failed.

#ifndef CHROME_BROWSER_BROWSER_PROCESS_IMPL_H_
#define CHROME_BROWSER_BROWSER_PROCESS_IMPL_H_

#include <string>

#include "base/basictypes.h"
#include "base/debug/stack_trace.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/prefs/pref_change_registrar.h"
#include "base/threading/non_thread_safe.h"
#include "base/timer/timer.h"
#include "chrome/browser/browser_process.h"

namespace base {
class CommandLine;
class SequencedTaskRunner;
}

// Real implementation of BrowserProcess that creates and returns the services.
class BrowserProcessImpl : public BrowserProcess,
                           public base::NonThreadSafe {
 public:
  // |local_state_task_runner| must be a shutdown-blocking task runner.
  BrowserProcessImpl(base::SequencedTaskRunner* local_state_task_runner,
                     const base::CommandLine& command_line);
  ~BrowserProcessImpl() override;

  // Called before the browser threads are created.
  void PreCreateThreads();

  // Called after the threads have been created but before the message loops
  // starts running. Allows the browser process to do any initialization that
  // requires all threads running.
  void PreMainMessageLoopRun();

  // Most cleanup is done by these functions, driven from
  // ChromeBrowserMain based on notifications from the content
  // framework, rather than in the destructor, so that we can
  // interleave cleanup with threads being stopped.
  void StartTearDown();
  void PostDestroyThreads();

  // BrowserProcess implementation.
  void EndSession() override;
  unsigned int AddRefModule() override;
  unsigned int ReleaseModule() override;
  bool IsShuttingDown() override;

 private:

  unsigned int module_ref_count_;
  bool did_start_;

  // TODO(eroman): Remove this when done debugging 113031. This tracks
  // the callstack which released the final module reference count.
  base::debug::StackTrace release_last_reference_callstack_;

  DISALLOW_COPY_AND_ASSIGN(BrowserProcessImpl);
};

#endif  // CHROME_BROWSER_BROWSER_PROCESS_IMPL_H_
