// Copyright by wangtianping. All rights reserved.

#include "content/content_switches.h"

namespace switches{
  // The value of this switch determines whether the process is started as a
  // renderer or plugin host.  If it's empty, it's the browser.
  const char kProcessType[] = "type";

  // Force logging to be enabled.  Logging is disabled by default in release
  // builds.
  const char kEnableLogging[] = "enable-logging";

  // Causes the browser process to crash on startup.
  const char kBrowserCrashTest[] = "crash-test";

  // Force logging to be disabled.  Logging is enabled by default in debug
  // builds.
  const char kDisableLogging[] = "disable-logging";

  // Sets the minimum log level. Valid values are from 0 to 3:
  // INFO = 0, WARNING = 1, LOG_ERROR = 2, LOG_FATAL = 3.
  const char kLoggingLevel[] = "log-level";

}
