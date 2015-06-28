// Copyright by wangtianping. All rights reserved.

#ifndef CONTENT_BROWSER_BROWSER_PROCESS_SUB_THREAD_H_
#define CONTENT_BROWSER_BROWSER_PROCESS_SUB_THREAD_H_

#include "base/basictypes.h"
#include "content/browser_thread_impl.h"
#include "content/content_export.h"

#if defined(OS_WIN)
namespace base {
namespace win {
class ScopedCOMInitializer;
}
}
#endif

namespace content {

// ----------------------------------------------------------------------------
// BrowserProcessSubThread
//
// This simple thread object is used for the specialized threads that the
// BrowserProcess spins up.
//
// Applications must initialize the COM library before they can call
// COM library functions other than CoGetMalloc and memory allocation
// functions, so this class initializes COM for those users.
class CONTENT_EXPORT BrowserProcessSubThread : public BrowserThreadImpl {
 public:
  explicit BrowserProcessSubThread(BrowserThread::ID identifier);
  ~BrowserProcessSubThread() override;

 protected:
  void Init() override;
  void CleanUp() override;

 private:
  scoped_ptr<base::win::ScopedCOMInitializer> com_initializer_;

  DISALLOW_COPY_AND_ASSIGN(BrowserProcessSubThread);
};

}  // namespace content

#endif  // CONTENT_BROWSER_BROWSER_PROCESS_SUB_THREAD_H_
