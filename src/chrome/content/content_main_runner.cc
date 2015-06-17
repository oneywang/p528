// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/content/content_main_runner.h"

#include <stdlib.h>

#include "base/allocator/allocator_extension.h"
#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/debug/debugger.h"
#include "base/files/file_path.h"
#include "base/i18n/icu_util.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "base/path_service.h"
#include "base/process/launch.h"
#include "base/process/memory.h"
#include "base/process/process_handle.h"
#include "base/profiler/alternate_timer.h"
#include "base/profiler/scoped_tracker.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event.h"
#include "chrome/content/browser_main.h"
//#include "content/common/set_process_title.h"
//#include "content/common/url_schemes.h"
//#include "content/gpu/in_process_gpu_thread.h"
#include "chrome/content/content_main.h"
#include "chrome/content/content_main_delegate.h"
#include "chrome/content/startup_helper_win.h"
#include "chrome/content/content_browser_client.h"
#include "chrome/content/content_client.h"
//#include "content/public/common/content_constants.h"
//#include "content/public/common/content_paths.h"
#include "chrome/common/content_switches.h"
#include "chrome/content/main_function_params.h"
//#include "content/public/common/sandbox_init.h"
//#include "content/renderer/in_process_renderer_thread.h"
//#include "content/utility/in_process_utility_thread.h"
//#include "crypto/nss_util.h"
//#include "ipc/ipc_descriptors.h"
//#include "ipc/ipc_switches.h"
//#include "media/base/media.h"
//#include "sandbox/win/src/sandbox_types.h"
//#include "ui/base/ui_base_paths.h"
//#include "ui/base/ui_base_switches.h"

#if defined(USE_TCMALLOC)
#include "third_party/tcmalloc/chromium/src/gperftools/malloc_extension.h"
#if defined(TYPE_PROFILING)
#include "base/allocator/type_profiler.h"
#include "base/allocator/type_profiler_tcmalloc.h"
#endif
#endif

#if defined(OS_WIN)
#include <malloc.h>
#include <cstring>

#include "base/strings/string_number_conversions.h"
#elif defined(OS_MACOSX)
#include "base/mac/scoped_nsautorelease_pool.h"
#if !defined(OS_IOS)
#endif  // !OS_IOS
#endif  // OS_WIN

#if !defined(OS_MACOSX) && defined(USE_TCMALLOC)
extern "C" {
int tc_set_new_mode(int mode);
}
#endif

namespace content {

base::LazyInstance<ContentBrowserClient>
    g_empty_content_browser_client = LAZY_INSTANCE_INITIALIZER;


void CommonSubprocessInit(const std::string& process_type) {
  // HACK: Let Windows know that we have started.  This is needed to suppress
  // the IDC_APPSTARTING cursor from being displayed for a prolonged period
  // while a subprocess is starting.
  PostThreadMessage(GetCurrentThreadId(), WM_NULL, 0, 0);
  MSG msg;
  PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
}

class ContentClientInitializer {
 public:
  static void Set(const std::string& process_type,
                  ContentMainDelegate* delegate) {
    ContentClient* content_client = GetContentClient();
    if (process_type.empty()) {
      if (delegate)
        content_client->browser_ = delegate->CreateContentBrowserClient();
      if (!content_client->browser_)
        content_client->browser_ = &g_empty_content_browser_client.Get();
    }
  }
};

// We dispatch to a process-type-specific FooMain() based on a command-line
// flag.  This struct is used to build a table of (flag, main function) pairs.
struct MainFunction {
  const char* name;
  int (*function)(const MainFunctionParams&);
};

// Run the FooMain() for a given process type.
// If |process_type| is empty, runs BrowserMain().
// Returns the exit code for this process.
int RunNamedProcessTypeMain(
    const std::string& process_type,
    const MainFunctionParams& main_function_params,
    ContentMainDelegate* delegate) {
  static const MainFunction kMainFunctions[] = {
    { "",                            BrowserMain },
  };

  for (size_t i = 0; i < arraysize(kMainFunctions); ++i) {
    if (process_type == kMainFunctions[i].name) {
      if (delegate) {
        int exit_code = delegate->RunProcess(process_type,
            main_function_params);
        if (exit_code >= 0)
          return exit_code;
      }
      return kMainFunctions[i].function(main_function_params);
    }
  }

  // If it's a process we don't know about, the embedder should know.
  if (delegate)
    return delegate->RunProcess(process_type, main_function_params);

  NOTREACHED() << "Unknown process type: " << process_type;
  return 1;
}

class ContentMainRunnerImpl : public ContentMainRunner {
 public:
  ContentMainRunnerImpl()
      : is_initialized_(false),
        is_shutdown_(false),
        completed_basic_startup_(false),
        delegate_(NULL),
        ui_task_(NULL) {
  }

  ~ContentMainRunnerImpl() override {
    if (is_initialized_ && !is_shutdown_)
      Shutdown();
  }

#if defined(USE_TCMALLOC)
  static bool GetAllocatorWasteSizeThunk(size_t* size) {
    size_t heap_size, allocated_bytes, unmapped_bytes;
    MallocExtension* ext = MallocExtension::instance();
    if (ext->GetNumericProperty("generic.heap_size", &heap_size) &&
        ext->GetNumericProperty("generic.current_allocated_bytes",
                                &allocated_bytes) &&
        ext->GetNumericProperty("tcmalloc.pageheap_unmapped_bytes",
                                &unmapped_bytes)) {
      *size = heap_size - allocated_bytes - unmapped_bytes;
      return true;
    }
    DCHECK(false);
    return false;
  }

  static void GetStatsThunk(char* buffer, int buffer_length) {
    MallocExtension::instance()->GetStats(buffer, buffer_length);
  }

  static void ReleaseFreeMemoryThunk() {
    MallocExtension::instance()->ReleaseFreeMemory();
  }
#endif

  int Initialize(const ContentMainParams& params) override {
    ui_task_ = params.ui_task;

    base::EnableTerminationOnOutOfMemory();
    RegisterInvalidParamHandler();
    //ui::win::CreateATLModuleIfNeeded();

    is_initialized_ = true;
    delegate_ = params.delegate;

    // The exit manager is in charge of calling the dtors of singleton objects.
    if (!ui_task_) {
      // When running browser tests, don't create a second AtExitManager as that
      // interfers with shutdown when objects created before ContentMain is
      // called are destructed when it returns.
      exit_manager_.reset(new base::AtExitManager);
    }

    // On Android, the command line is initialized when library is loaded and
    // we have already started our TRACE_EVENT0.
    // argc/argv are ignored on Windows and Android; see command_line.h for
    // details.
    int argc = 0;
    const char** argv = NULL;

    base::CommandLine::Init(argc, argv);

    base::EnableTerminationOnHeapCorruption();

    int exit_code = 0;
    if (delegate_ && delegate_->BasicStartupComplete(&exit_code))
      return exit_code;

    completed_basic_startup_ = true;

    const base::CommandLine& command_line =
        *base::CommandLine::ForCurrentProcess();
    std::string process_type =
        command_line.GetSwitchValueASCII(switches::kProcessType);

    if (!GetContentClient())
      SetContentClient(&empty_content_client_);
    ContentClientInitializer::Set(process_type, delegate_);

#if defined(OS_WIN)
    // Route stdio to parent console (if any) or create one.
    if (command_line.HasSwitch(switches::kEnableLogging))
      base::RouteStdioToConsole();
#endif

    // Enable startup tracing asap to avoid early TRACE_EVENT calls being
    // ignored.
    if (command_line.HasSwitch(switches::kTraceStartup)) {
      base::trace_event::CategoryFilter category_filter(
          command_line.GetSwitchValueASCII(switches::kTraceStartup));
      base::trace_event::TraceLog::GetInstance()->SetEnabled(
          category_filter,
          base::trace_event::TraceLog::RECORDING_MODE,
          base::trace_event::TraceOptions(
              base::trace_event::RECORD_UNTIL_FULL));
    }

    SetupCRT(command_line);

    //ui::RegisterPathProvider();
    //RegisterPathProvider();
    //RegisterContentSchemes(true);

    if (!process_type.empty())
      CommonSubprocessInit(process_type);

    // Return -1 to indicate no early termination.
    return -1;
  }

  int Run() override {
    DCHECK(is_initialized_);
    DCHECK(!is_shutdown_);
    const base::CommandLine& command_line =
        *base::CommandLine::ForCurrentProcess();
    std::string process_type =
        command_line.GetSwitchValueASCII(switches::kProcessType);

    MainFunctionParams main_params(command_line);
    main_params.ui_task = ui_task_;
    return RunNamedProcessTypeMain(process_type, main_params, delegate_);
  }

  void Shutdown() override {
    DCHECK(is_initialized_);
    DCHECK(!is_shutdown_);

    if (completed_basic_startup_ && delegate_) {
      const base::CommandLine& command_line =
          *base::CommandLine::ForCurrentProcess();
      std::string process_type =
          command_line.GetSwitchValueASCII(switches::kProcessType);

      delegate_->ProcessExiting(process_type);
    }

#if defined(OS_WIN)
#ifdef _CRTDBG_MAP_ALLOC
    _CrtDumpMemoryLeaks();
#endif  // _CRTDBG_MAP_ALLOC
#endif  // OS_WIN

#if defined(OS_MACOSX) && !defined(OS_IOS)
    autorelease_pool_.reset(NULL);
#endif

    exit_manager_.reset(NULL);

    delegate_ = NULL;
    is_shutdown_ = true;
  }

 private:
  // True if the runner has been initialized.
  bool is_initialized_;

  // True if the runner has been shut down.
  bool is_shutdown_;

  // True if basic startup was completed.
  bool completed_basic_startup_;

  // Used if the embedder doesn't set one.
  ContentClient empty_content_client_;

  // The delegate will outlive this object.
  ContentMainDelegate* delegate_;

  scoped_ptr<base::AtExitManager> exit_manager_;

  base::Closure* ui_task_;

  DISALLOW_COPY_AND_ASSIGN(ContentMainRunnerImpl);
};

// static
ContentMainRunner* ContentMainRunner::Create() {
  return new ContentMainRunnerImpl();
}

}  // namespace content
