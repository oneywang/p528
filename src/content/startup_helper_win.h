// Copyright by wangtianping. All rights reserved.

#ifndef CONTENT_PUBLIC_APP_STARTUP_HELPER_WIN_H_
#define CONTENT_PUBLIC_APP_STARTUP_HELPER_WIN_H_

namespace base {
class CommandLine;
}

// This file contains functions that any embedder that's not using ContentMain
// will want to call at startup.
// NOTE: we never want to CONTENT_EXPORT these functions, they must run in the
// same module that calls them.
namespace content {

// Register the invalid param handler and pure call handler to be able to
// notify breakpad when it happens.
void RegisterInvalidParamHandler();

// Sets up the CRT's debugging macros to output to stdout.
void SetupCRT(const base::CommandLine& command_line);

}  // namespace content

#endif  // CONTENT_PUBLIC_APP_STARTUP_HELPER_WIN_H_
