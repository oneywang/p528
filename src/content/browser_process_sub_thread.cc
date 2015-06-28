// Copyright by wangtianping. All rights reserved.

#include "content/browser_process_sub_thread.h"

#include "base/debug/leak_tracker.h"
#include "base/threading/thread_restrictions.h"
#include "base/win/scoped_com_initializer.h"
#include "build/build_config.h"

namespace content {

BrowserProcessSubThread::BrowserProcessSubThread(BrowserThread::ID identifier)
    : BrowserThreadImpl(identifier) {
}

BrowserProcessSubThread::~BrowserProcessSubThread() {
  Stop();
}

void BrowserProcessSubThread::Init() {
  com_initializer_.reset(new base::win::ScopedCOMInitializer());

  BrowserThreadImpl::Init();

  if (BrowserThread::CurrentlyOn(BrowserThread::IO)) {
    // Though this thread is called the "IO" thread, it actually just routes
    // messages around; it shouldn't be allowed to perform any blocking disk
    // I/O.
    base::ThreadRestrictions::SetIOAllowed(false);
    base::ThreadRestrictions::DisallowWaiting();
  }
}

void BrowserProcessSubThread::CleanUp() {
  BrowserThreadImpl::CleanUp();

  com_initializer_.reset();
}

}  // namespace content
