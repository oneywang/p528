// Copyright by wangtianping. All rights reserved.

#include "chrome/app/chrome_env_vars.h"

namespace env_vars {

// CHROME_CRASHED exists if a previous instance of chrome has crashed. This
// triggers the 'restart chrome' dialog. CHROME_RESTART contains the strings
// that are needed to show the dialog.
const char kShowRestart[] = "CHROME_CRASHED";
const char kRestartInfo[] = "CHROME_RESTART";

// The strings RIGHT_TO_LEFT and LEFT_TO_RIGHT indicate the locale direction.
// For example, for Hebrew and Arabic locales, we use RIGHT_TO_LEFT so that the
// dialog is displayed using the right orientation.
const char kRtlLocale[] = "RIGHT_TO_LEFT";
const char kLtrLocale[] = "LEFT_TO_RIGHT";

}  // namespace env_vars
