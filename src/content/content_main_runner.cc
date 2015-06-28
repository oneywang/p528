// Copyright by wangtianping. All rights reserved.

#include "content/content_main_runner.h"

#include <cstring>
#include <malloc.h>
#include <stdlib.h>

#include "base/allocator/allocator_extension.h"
#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/debug/debugger.h"
#include "base/files/file_path.h"
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
#include "content/browser_main.h"
#include "content/content_main.h"
#include "content/content_main_delegate.h"
#include "content/startup_helper_win.h"
#include "content/content_browser_client.h"
#include "content/content_client.h"
#include "content/content_switches.h"
#include "content/main_function_params.h"

#if defined(USE_TCMALLOC)
#include "third_party/tcmalloc/chromium/src/gperftools/malloc_extension.h"
#endif

#if defined(USE_TCMALLOC)
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
        delegate_(NULL){
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
    base::EnableTerminationOnOutOfMemory();
    RegisterInvalidParamHandler();

#if defined(USE_TCMALLOC)
    // For tcmalloc, we need to tell it to behave like new.
    tc_set_new_mode(1);

    // On windows, we've already set these thunks up in _heap_init()
    base::allocator::SetGetAllocatorWasteSizeFunction(GetAllocatorWasteSizeThunk);
    base::allocator::SetGetStatsFunction(GetStatsThunk);
    base::allocator::SetReleaseFreeMemoryFunction(ReleaseFreeMemoryThunk);
#endif

    is_initialized_ = true;
    delegate_ = params.delegate;

    // When running browser tests, don't create a second AtExitManager as that
    // interfers with shutdown when objects created before ContentMain is
    // called are destructed when it returns.
    exit_manager_.reset(new base::AtExitManager);

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

    // Route stdio to parent console (if any) or create one.
    if (command_line.HasSwitch(switches::kEnableLogging)){
      base::RouteStdioToConsole();
    }

    SetupCRT(command_line);

    if (!process_type.empty()) CommonSubprocessInit(process_type);

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

#ifdef _CRTDBG_MAP_ALLOC
    _CrtDumpMemoryLeaks();
#endif  // _CRTDBG_MAP_ALLOC

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

  DISALLOW_COPY_AND_ASSIGN(ContentMainRunnerImpl);
};

// static
ContentMainRunner* ContentMainRunner::Create() {
  return new ContentMainRunnerImpl();
}

}  // namespace content
