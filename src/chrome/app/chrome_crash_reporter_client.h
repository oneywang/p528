// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_APP_CHROME_CRASH_REPORTER_CLIENT_H_
#define CHROME_APP_CHROME_CRASH_REPORTER_CLIENT_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "chrome/breakpad/crash_reporter_client.h"

namespace base {
  class CommandLine;
}

namespace chrome {

class ChromeCrashReporterClient : public crash_reporter::CrashReporterClient {
 public:
  ChromeCrashReporterClient();
  ~ChromeCrashReporterClient() override;

  // crash_reporter::CrashReporterClient implementation.
  virtual bool ShouldShowRestartDialog(base::string16* title,
                                       base::string16* message,
                                       bool* is_rtl_locale) override;
  virtual bool AboutToRestart() override;
  bool EnableBreakpadForProcess(const std::string& process_type) override;
  virtual bool GetShouldDumpLargerDumps(bool is_per_user_install) override;

  // helper
  static void PrepareRestartOnCrashEnviroment(const base::CommandLine& parsed_command_line);

 private:

  DISALLOW_COPY_AND_ASSIGN(ChromeCrashReporterClient);
};

}  // namespace chrome

#endif  // CHROME_APP_CHROME_CRASH_REPORTER_CLIENT_H_
