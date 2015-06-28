// Copyright by wangtianping. All rights reserved.

#include "mdgate/app/chrome_main_delegate.h"

#include "base/debug/dump_without_crashing.h"
#include "base/win/win_util.h"
#include "content/content_main.h"

#define DLLEXPORT __declspec(dllexport)

// We use extern C for the prototype DLLEXPORT to avoid C++ name mangling.
extern "C" {
DLLEXPORT int __cdecl ChromeMain(HINSTANCE instance,
                                 void* sandbox_info);
}

DLLEXPORT int __cdecl ChromeMain(HINSTANCE instance,
                                 void* sandbox_info) {
  ChromeMainDelegate chrome_main_delegate;
  content::ContentMainParams params(&chrome_main_delegate);

  // The process should crash when going through abnormal termination.
  base::win::SetShouldCrashOnProcessDetach(true);
  base::win::SetAbortBehaviorForCrashReporting();
  params.instance = instance;

  // SetDumpWithoutCrashingFunction must be passed the DumpProcess function
  // from the EXE and not from the DLL in order for DumpWithoutCrashing to
  // function correctly.
  typedef void (__cdecl *DumpProcessFunction)();
  DumpProcessFunction DumpProcess = reinterpret_cast<DumpProcessFunction>(
      ::GetProcAddress(::GetModuleHandle(L"chrome.exe"),
          "DumpProcessWithoutCrash"));
  base::debug::SetDumpWithoutCrashingFunction(DumpProcess);

  int rv = content::ContentMain(params);

  base::win::SetShouldCrashOnProcessDetach(false);

  return rv;
}
