// Copyright by wangtianping. All rights reserved.

#include "chrome/browser/browser_process_impl.h"

#include <algorithm>
#include <map>
#include <vector>

#include "base/atomic_ref_count.h"
#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/command_line.h"
#include "base/debug/alias.h"
#include "base/debug/leak_annotations.h"
#include "base/files/file_path.h"
#include "base/path_service.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "base/time/default_tick_clock.h"
#include "base/trace_event/trace_event.h"
#include "chrome/browser/chrome_browser_main.h"
#include "chrome/browser/chrome_content_browser_client.h"
#include "chrome/common/chrome_constants.h"
#include "content/browser_thread.h"

// How long to wait for the File thread to complete during EndSession, on Linux
// and Windows. We have a timeout here because we're unable to run the UI
// messageloop and there's some deadlock risk. Our only option is to exit
// anyway.
static const int kEndSessionTimeoutSeconds = 10;

using content::BrowserThread;

BrowserProcessImpl::BrowserProcessImpl(
    base::SequencedTaskRunner* local_state_task_runner,
    const base::CommandLine& command_line)
    : module_ref_count_(0),
      did_start_(false){
  g_browser_process = this;
}

BrowserProcessImpl::~BrowserProcessImpl() {
  g_browser_process = NULL;
}

void BrowserProcessImpl::StartTearDown() {
}

void BrowserProcessImpl::PostDestroyThreads() {
}

unsigned int BrowserProcessImpl::AddRefModule() {
  DCHECK(CalledOnValidThread());

  if (IsShuttingDown()) {
    // Copy the stacktrace which released the final reference onto our stack so
    // it will be available in the crash report for inspection.
    base::debug::StackTrace callstack = release_last_reference_callstack_;
    base::debug::Alias(&callstack);
    CHECK(false);
  }

  did_start_ = true;
  module_ref_count_++;
  return module_ref_count_;
}

unsigned int BrowserProcessImpl::ReleaseModule() {
  DCHECK(CalledOnValidThread());
  DCHECK_NE(0u, module_ref_count_);
  module_ref_count_--;
  if (0 == module_ref_count_) {
    release_last_reference_callstack_ = base::debug::StackTrace();

    CHECK(base::MessageLoop::current()->is_running());

    base::MessageLoop::current()->Quit();
  }
  return module_ref_count_;
}

namespace {

// Used at the end of session to block the UI thread for completion of sentinel
// tasks on the set of threads used to persist profile data and local state.
// This is done to ensure that the data has been persisted to disk before
// continuing.
class RundownTaskCounter :
    public base::RefCountedThreadSafe<RundownTaskCounter> {
 public:
  RundownTaskCounter();

  // Posts a rundown task to |task_runner|, can be invoked an arbitrary number
  // of times before calling TimedWait.
  void Post(base::SequencedTaskRunner* task_runner);

  // Waits until the count is zero or |max_time| has passed.
  // This can only be called once per instance.
  bool TimedWait(const base::TimeDelta& max_time);

 private:
  friend class base::RefCountedThreadSafe<RundownTaskCounter>;
  ~RundownTaskCounter() {}

  // Decrements the counter and releases the waitable event on transition to
  // zero.
  void Decrement();

  // The count starts at one to defer the possibility of one->zero transitions
  // until TimedWait is called.
  base::AtomicRefCount count_;
  base::WaitableEvent waitable_event_;

  DISALLOW_COPY_AND_ASSIGN(RundownTaskCounter);
};

RundownTaskCounter::RundownTaskCounter()
    : count_(1), waitable_event_(true, false) {
}

void RundownTaskCounter::Post(base::SequencedTaskRunner* task_runner) {
  // As the count starts off at one, it should never get to zero unless
  // TimedWait has been called.
  DCHECK(!base::AtomicRefCountIsZero(&count_));

  base::AtomicRefCountInc(&count_);

  // The task must be non-nestable to guarantee that it runs after all tasks
  // currently scheduled on |task_runner| have completed.
  task_runner->PostNonNestableTask(FROM_HERE,
      base::Bind(&RundownTaskCounter::Decrement, this));
}

void RundownTaskCounter::Decrement() {
  if (!base::AtomicRefCountDec(&count_))
    waitable_event_.Signal();
}

bool RundownTaskCounter::TimedWait(const base::TimeDelta& max_time) {
  // Decrement the excess count from the constructor.
  Decrement();

  return waitable_event_.TimedWait(max_time);
}

}  // namespace

void BrowserProcessImpl::EndSession() {
  // http://crbug.com/125207
  base::ThreadRestrictions::ScopedAllowWait allow_wait;
  scoped_refptr<RundownTaskCounter> rundown_counter(new RundownTaskCounter());

  // We must write that the profile and metrics service shutdown cleanly,
  // otherwise on startup we'll think we crashed. So we block until done and
  // then proceed with normal shutdown.
  //
  // If you change the condition here, be sure to also change
  // ProfileBrowserTests to match.

  // Do a best-effort wait on the successful countdown of rundown tasks. Note
  // that if we don't complete "quickly enough", Windows will terminate our
  // process.
  //
  // On Windows, we previously posted a message to FILE and then ran a nested
  // message loop, waiting for that message to be processed until quitting.
  // However, doing so means that other messages will also be processed. In
  // particular, if the GPU process host notices that the GPU has been killed
  // during shutdown, it races exiting the nested loop with the process host
  // blocking the message loop attempting to re-establish a connection to the
  // GPU process synchronously. Because the system may not be allowing
  // processes to launch, this can result in a hang. See
  // http://crbug.com/318527.
  rundown_counter->TimedWait(
      base::TimeDelta::FromSeconds(kEndSessionTimeoutSeconds));
}

bool BrowserProcessImpl::IsShuttingDown() {
  DCHECK(CalledOnValidThread());
  return did_start_ && 0 == module_ref_count_;
}

void BrowserProcessImpl::PreCreateThreads() {
}

void BrowserProcessImpl::PreMainMessageLoopRun() {
}

