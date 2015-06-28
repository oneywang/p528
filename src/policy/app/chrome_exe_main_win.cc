// Copyright by wangtianping. All rights reserved.

#include <windows.h>
#include <malloc.h>
#include <shellscalingapi.h>
#include <tchar.h>

#include <string>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/win/windows_version.h"
#include "chrome/app/client_util.h"
#include "content/result_codes.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prev, wchar_t*, int) {
  // Initialize the commandline singleton from the environment.
  base::CommandLine::Init(0, NULL);
  // The exit manager is in charge of calling the dtors of singletons.
  base::AtExitManager exit_manager;

  // Load and launch the chrome dll. *Everything* happens inside.
  MainDllLoader* loader = MakeMainDllLoader();
  int rc = loader->Launch(instance);
  delete loader;
  return rc;
}
