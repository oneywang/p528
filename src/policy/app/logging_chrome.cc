// Copyright by wangtianping. All rights reserved.

#include "chrome/app/logging_chrome.h"

#include <initguid.h>
#include <windows.h>
#include <fstream>  // NOLINT
#include <string>  // NOLINT

#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/debug/debugger.h"
#include "base/debug/dump_without_crashing.h"
#include "base/environment.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging_win.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread_restrictions.h"
#include "build/build_config.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/chrome_env_vars.h"

namespace {

// When true, this means that error dialogs should not be shown.
bool dialogs_are_suppressed_ = false;

// This should be true for exactly the period between the end of
// InitChromeLogging() and the beginning of CleanupChromeLogging().
bool chrome_logging_initialized_ = false;

// Set if we called InitChromeLogging() but failed to initialize.
bool chrome_logging_failed_ = false;

// This should be true for exactly the period between the end of
// InitChromeLogging() and the beginning of CleanupChromeLogging().
bool chrome_logging_redirected_ = false;

// Assertion handler for logging errors that occur when dialogs are
// silenced.  To record a new error, pass the log string associated
// with that error in the str parameter.
MSVC_DISABLE_OPTIMIZE();
void SilentRuntimeAssertHandler(const std::string& str) {
  base::debug::BreakDebugger();
}
MSVC_ENABLE_OPTIMIZE();

// Suppresses error/assertion dialogs and enables the logging of
// those errors into silenced_errors_.
void SuppressDialogs() {
  if (dialogs_are_suppressed_)
    return;

  logging::SetLogAssertHandler(SilentRuntimeAssertHandler);

  UINT new_flags = SEM_FAILCRITICALERRORS |
                   SEM_NOGPFAULTERRORBOX |
                   SEM_NOOPENFILEERRORBOX;

  // Preserve existing error mode, as discussed at http://t/dmea
  UINT existing_flags = SetErrorMode(new_flags);
  SetErrorMode(existing_flags | new_flags);

  dialogs_are_suppressed_ = true;
}

}  // anonymous namespace

namespace logging {

LoggingDestination DetermineLogMode(const base::CommandLine& command_line) {
  // only use OutputDebugString in debug mode
#ifdef XXXXXX //NDEBUG
  bool enable_logging = false;
  const char *kInvertLoggingSwitch = switches::kEnableLogging;
  const logging::LoggingDestination kDefaultLoggingMode = logging::LOG_TO_FILE;
#else
  bool enable_logging = true;
  const char *kInvertLoggingSwitch = switches::kDisableLogging;
  const logging::LoggingDestination kDefaultLoggingMode = logging::LOG_TO_ALL;
#endif

  if (command_line.HasSwitch(kInvertLoggingSwitch))
    enable_logging = !enable_logging;

  logging::LoggingDestination log_mode;
  if (enable_logging) {
    // Let --enable-logging=stderr force only stderr, particularly useful for
    // non-debug builds where otherwise you can't get logs to stderr at all.
    if (command_line.GetSwitchValueASCII(switches::kEnableLogging) == "stderr")
      log_mode = logging::LOG_TO_SYSTEM_DEBUG_LOG;
    else
      log_mode = kDefaultLoggingMode;
  } else {
    log_mode = logging::LOG_NONE;
  }
  return log_mode;
}

void InitChromeLogging(const base::CommandLine& command_line,
                       OldFileDeletionState delete_old_log_file) {
  DCHECK(!chrome_logging_initialized_) <<
    "Attempted to initialize logging when it was already initialized.";

  LoggingDestination logging_dest = DetermineLogMode(command_line);
  LogLockingState log_locking_state = LOCK_LOG_FILE;
  base::FilePath log_path;

  // Don't resolve the log path unless we need to. Otherwise we leave an open
  // ALPC handle after sandbox lockdown on Windows.
  if ((logging_dest & LOG_TO_FILE) != 0) {
    log_path = GetLogFileName();
  } else {
    log_locking_state = DONT_LOCK_LOG_FILE;
  }

  logging::LoggingSettings settings;
  settings.logging_dest = logging_dest;
  settings.log_file = log_path.value().c_str();
  settings.lock_log = log_locking_state;
  settings.delete_old = delete_old_log_file;
  bool success = logging::InitLogging(settings);
  if (!success) {
    DPLOG(ERROR) << "Unable to initialize logging to " << log_path.value();
    chrome_logging_failed_ = true;
    return;
  }

  // Default to showing error dialogs.
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kNoErrorDialogs)){
    logging::SetShowErrorDialogs(true);
  }
  // we want process and thread IDs because we have a lot of things running
  logging::SetLogItems(true,  // enable_process_id
                       true,  // enable_thread_id
                       true,  // enable_timestamp
                       false);  // enable_tickcount

  // We call running in unattended mode "headless", and allow
  // headless mode to be configured either by the Environment
  // Variable or by the Command Line Switch.  This is for
  // automated test purposes.
  scoped_ptr<base::Environment> env(base::Environment::Create());
  if (env->HasVar(env_vars::kHeadless) ||
    command_line.HasSwitch(switches::kNoErrorDialogs)){
    SuppressDialogs();
  }

  // Use a minimum log level if the command line asks for one,
  // otherwise leave it at the default level (INFO).
  if (command_line.HasSwitch(switches::kLoggingLevel)) {
    std::string log_level = command_line.GetSwitchValueASCII(
        switches::kLoggingLevel);
    int level = 0;
    if (base::StringToInt(log_level, &level) &&
        level >= 0 && level < LOG_NUM_SEVERITIES) {
      logging::SetMinLogLevel(level);
    } else {
      DLOG(WARNING) << "Bad log level: " << log_level;
    }
  }

  chrome_logging_initialized_ = true;
}

// This is a no-op, but we'll keep it around in case
// we need to do more cleanup in the future.
void CleanupChromeLogging() {
  if (chrome_logging_failed_)
    return;  // We failed to initiailize logging, no cleanup.

  DCHECK(chrome_logging_initialized_) <<
    "Attempted to clean up logging when it wasn't initialized.";

  CloseLogFile();

  chrome_logging_initialized_ = false;
  chrome_logging_redirected_ = false;
}

base::FilePath GetLogFileName() {
  std::string filename;
  scoped_ptr<base::Environment> env(base::Environment::Create());
  if (env->GetVar(env_vars::kLogFileName, &filename) && !filename.empty())
    return base::FilePath::FromUTF8Unsafe(filename);

  const base::FilePath log_filename(FILE_PATH_LITERAL("chrome_debug.log"));
  return log_filename;
}

bool DialogsAreSuppressed() {
  return dialogs_are_suppressed_;
}
base::FilePath GenerateTimestampedName(const base::FilePath& base_path,
                                       base::Time timestamp) {
  base::Time::Exploded time_deets;
  timestamp.LocalExplode(&time_deets);
  std::string suffix = base::StringPrintf("_%02d%02d%02d-%02d%02d%02d",
                                          time_deets.year,
                                          time_deets.month,
                                          time_deets.day_of_month,
                                          time_deets.hour,
                                          time_deets.minute,
                                          time_deets.second);
  return base_path.InsertBeforeExtensionASCII(suffix);
}

}  // namespace logging
