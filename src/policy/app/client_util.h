// Copyright by wangtianping. All rights reserved.

// This file defines utility functions that can report details about the
// host operating environment.

#ifndef CHROME_APP_CLIENT_UTIL_H_
#define CHROME_APP_CLIENT_UTIL_H_

#include <windows.h>

#include <string>
#include "base/strings/string16.h"
#include "base/files/file_path.h"

// Returns the version in the current module's version resource or the empty
// string if none found.
base::string16 GetCurrentModuleVersion();

// Implements the common aspects of loading the main dll for both chrome and
// chromium scenarios, which are in charge of implementing two abstract
// methods: GetRegistryPath() and OnBeforeLaunch().
class MainDllLoader {
 public:
  MainDllLoader();
  virtual ~MainDllLoader();

  // Loads and calls the entry point of chrome.dll. |instance| is the exe
  // instance retrieved from wWinMain.
  // The return value is what the main entry point of chrome.dll returns
  // upon termination.
  int Launch(HINSTANCE instance);

 private:
  // Loads chrome.dll, populating |version| with the version of the DLL loaded
  // and |module| with the path of the loaded DLL. Returns a reference to the
  // module, or null on failure.
  HMODULE Load(base::string16* version, base::FilePath* module);

 private:
  HMODULE dll_;
  std::string process_type_;
};

// Factory for the MainDllLoader. Caller owns the pointer and should call
// delete to free it.
MainDllLoader* MakeMainDllLoader();

#endif  // CHROME_APP_CLIENT_UTIL_H_
