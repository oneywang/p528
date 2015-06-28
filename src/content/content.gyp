# Copyright by wangtianping. All Rights Reserved.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    { #content_static.lib
      'target_name': 'content_static',
      'type': 'static_library',
      'sources': [
        'content_switches.cc',
        'content_switches.h',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
      ],      
    },
    { #content component
      'target_name': 'content',
      'type': '<(component)',
      'sources': [
        'browser_main.cc',
        'browser_main.h',
        'browser_main_loop.cc',
        'browser_main_loop.h',
        'browser_main_parts.cc',
        'browser_main_parts.h',
        'browser_main_runner.cc',
        'browser_main_runner.h',
        'browser_process_sub_thread.cc',
        'browser_process_sub_thread.h',
        'browser_shutdown.h',
        'browser_thread.h',
        'browser_thread_delegate.h',
        'browser_thread_impl.cc',
        'browser_thread_impl.h',
        'content_browser_client.cc',
        'content_browser_client.h',
        'content_client.cc',
        'content_client.h',
        'content_export.h',
        'content_main.cc',
        'content_main.h',
        'content_main_delegate.cc',
        'content_main_delegate.h',
        'content_main_runner.cc',
        'content_main_runner.h',
        'main_function_params.h',
        'startup_helper_win.cc',
        'startup_helper_win.h',
        'startup_task_runner.cc',
        'startup_task_runner.h',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/base.gyp:base_static',
        'content_static',
      ],
      'defines': [
        'CONTENT_IMPLEMENTATION',
      ],
      'msvs_settings': {
        'VCLinkerTool': {
          'AdditionalDependencies': [
            'imm32.lib',
            'comctl32.lib',
          ],         
        },
      },
    },
  ],#targets
}
