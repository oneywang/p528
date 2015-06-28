// Copyright by wangtianping. All rights reserved.

#ifndef CONTENT_PUBLIC_APP_CONTENT_MAIN_DELEGATE_H_
#define CONTENT_PUBLIC_APP_CONTENT_MAIN_DELEGATE_H_

#include <string>

#include "build/build_config.h"
#include "content/content_export.h"

template <typename>
class ScopedVector;

namespace content {

class ContentBrowserClient;
struct MainFunctionParams;

class CONTENT_EXPORT ContentMainDelegate {
 public:
  virtual ~ContentMainDelegate() {}

  // Tells the embedder that the absolute basic startup has been done, i.e.
  // it's now safe to create singletons and check the command line. Return true
  // if the process should exit afterwards, and if so, |exit_code| should be
  // set. This is the place for embedder to do the things that must happen at
  // the start. Most of its startup code should be in the methods below.
  virtual bool BasicStartupComplete(int* exit_code);

  // Asks the embedder to start a process. Return -1 for the default behavior.
  virtual int RunProcess(
    const std::string& process_type,
    const MainFunctionParams& main_function_params);

  // Called right before the process exits.
  virtual void ProcessExiting(const std::string& process_type) {}

 protected:
  friend class ContentClientInitializer;

  // Called once per relevant process type to allow the embedder to customize
  // content. If an embedder wants the default (empty) implementation, don't
  // override this.
  virtual ContentBrowserClient* CreateContentBrowserClient();
};

}  // namespace content

#endif  // CONTENT_PUBLIC_APP_CONTENT_MAIN_DELEGATE_H_
