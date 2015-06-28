// Copyright by wangtianping. All rights reserved.

#include "chrome/app/chrome_crash_reporter_client.h"

#include "base/command_line.h"
#include "base/environment.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/app/chrome_env_vars.h"
#include "content/content_switches.h"

namespace chrome {


ChromeCrashReporterClient::ChromeCrashReporterClient() {
}

ChromeCrashReporterClient::~ChromeCrashReporterClient() {}

bool ChromeCrashReporterClient::ShouldShowRestartDialog(base::string16* title,
                                                        base::string16* message,
                                                        bool* is_rtl_locale) {
  scoped_ptr<base::Environment> env(base::Environment::Create());
  if (!env->HasVar(env_vars::kShowRestart) ||
      !env->HasVar(env_vars::kRestartInfo)) {
    return false;
  }

  std::string restart_info;
  env->GetVar(env_vars::kRestartInfo, &restart_info);

  // The CHROME_RESTART var contains the dialog strings separated by '|'.
  // See ChromeBrowserMainPartsWin::PrepareRestartOnCrashEnviroment()
  // for details.
  std::vector<std::string> dlg_strings;
  base::SplitString(restart_info, '|', &dlg_strings);

  if (dlg_strings.size() < 3)
    return false;

  *title = base::UTF8ToUTF16(dlg_strings[0]);
  *message = base::UTF8ToUTF16(dlg_strings[1]);
  *is_rtl_locale = dlg_strings[2] == env_vars::kRtlLocale;
  return true;
}

bool ChromeCrashReporterClient::AboutToRestart() {
  scoped_ptr<base::Environment> env(base::Environment::Create());
  if (!env->HasVar(env_vars::kRestartInfo))
    return false;

  env->SetVar(env_vars::kShowRestart, "1");
  return true;
}

bool ChromeCrashReporterClient::EnableBreakpadForProcess(
    const std::string& process_type) {
  return process_type == "browser";
}

bool ChromeCrashReporterClient::GetShouldDumpLargerDumps(bool is_per_user_install) {
  return true;
}

// static
void ChromeCrashReporterClient::PrepareRestartOnCrashEnviroment(
  const base::CommandLine& parsed_command_line) {
  // Clear this var so child processes don't show the dialog by default.
  scoped_ptr<base::Environment> env(base::Environment::Create());
  env->UnSetVar(env_vars::kShowRestart);

  // If the known command-line test options are used we don't create the
  // environment block which means we don't get the restart dialog.
  if (parsed_command_line.HasSwitch(switches::kBrowserCrashTest) ||
    parsed_command_line.HasSwitch(switches::kNoErrorDialogs))
    return;

  // The encoding we use for the info is "title|context|direction" where
  // direction is either env_vars::kRtlLocale or env_vars::kLtrLocale depending
  // on the current locale.
  base::string16 dlg_strings(L"警告");
  dlg_strings.push_back('|');
  base::string16 adjusted_string(L"软件崩溃了，已帮您重新运行，是否继续？");
  //base::i18n::AdjustStringForLocaleDirection(&adjusted_string);
  dlg_strings.append(adjusted_string);
  dlg_strings.push_back('|');
  dlg_strings.append(base::ASCIIToUTF16(env_vars::kLtrLocale));

  env->SetVar(env_vars::kRestartInfo, base::UTF16ToUTF8(dlg_strings));
}
}  // namespace chrome
