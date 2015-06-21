# Copyright 2012 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

{
  'variables': {
    'chromium_code': 1,
    'variables': {    
      'variables': {
        'conditions': [
          ['component=="shared_library"', {
            'qt_sdk%': 'C:/qtbase/qt5.5-vs12-shared',
            'qt_defines%': [
              'QT_NO_DEBUG',
              'QT_WIDGETS_LIB',
              'QT_GUI_LIB',
              'QT_CORE_LIB',
              'QT_SHARED',
            ],       
          },{
           'qt_sdk%': 'C:/qtbase/qt5.5-vs12',
           'qt_defines%': [
              'QT_NO_DEBUG',
              'QT_WIDGETS_LIB',
              'QT_GUI_LIB',
              'QT_CORE_LIB',
              'QT_STATICPLUGIN',
              'QT_STATIC',
            ],
          },],
        ],   
      },
      'qt_defines%': '<(qt_defines)',
      'qt_sdk%': '<(qt_sdk)',
      'conditions': [
        ['component=="shared_library"', {
          'qt_libs%': [
            '<(qt_sdk)/lib/Qt5Core.lib',
            '<(qt_sdk)/lib/Qt5Gui.lib',
            '<(qt_sdk)/lib/Qt5Widgets.lib',
          ],
        },{
          'qt_libs%': [
            '<(qt_sdk)/lib/Qt5Core.lib',
            '<(qt_sdk)/lib/Qt5Gui.lib',
            '<(qt_sdk)/lib/Qt5Widgets.lib',
            '<(qt_sdk)/lib/Qt5PlatformSupport.lib',
            '<(qt_sdk)/lib/qtpcre.lib',
            '<(qt_sdk)/lib/qtharfbuzzng.lib',            
            '<(qt_sdk)/plugins/platforms/qwindows.lib',
            '<(qt_sdk)/plugins/imageformats/qico.lib',
          ],
        },],
      ],
    },
    'qt_defines%': '<(qt_defines)',
    'qt_sdk%': '<(qt_sdk)',    
    'qt_libs%': '<(qt_libs)',    
    'qt_includes': [
       '<(qt_sdk)/include',
       '<(qt_sdk)/include/QtCore',
       '<(qt_sdk)/include/QtGui',
       '<(qt_sdk)/include/QtWidgets',
     ],
  },
  #disalbe showincludes in targets but invalid,modify gyp/ninja.py by hand!!!
  'target_defaults': {
    'msvs_settings': {
      'VCCLCompilerTool': {
        'ShowIncludes': 'false'
      },
    },
  },
  'targets': [
    { #All
      'target_name': '__base__',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base_unittests',
      ],
    },
    { #chrome.exe
      'target_name': 'chrome_exe',
      'type': 'executable',
      'product_name': 'chrome',
      'sources': [
        'app/chrome_exe_main_win.cc',
        'app/client_util.cc',
        'app/client_util.h',
        'app/image_pre_reader_win.cc',
        'app/image_pre_reader_win.h',
        'app/chrome_crash_reporter_client.cc',
        'app/chrome_crash_reporter_client.h',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/base.gyp:base_static',
        '<(DEPTH)/breakpad/breakpad.gyp:breakpad_handler',
        'cr_common',
        'chrome_dll',
        'cr_breakpad',
      ],      
      'msvs_settings': {
        'VCLinkerTool': {
          'SubSystem': '2',         # Set /SUBSYSTEM:WINDOWS
        },
      },
    },
    { #cr_common.lib
      'target_name': 'cr_common',
      'type': 'static_library',
      'sources': [
        'common/chrome_constants.cc',
        'common/chrome_constants.h',
        'common/chrome_result_codes.h',
        'common/chrome_switches.cc',
        'common/chrome_switches.h',
        'common/content_switches.cc',
        'common/content_switches.h',
        'common/env_vars.cc',
        'common/env_vars.h',
        'common/result_codes.h',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
      ],
    },
    { #chrome.dll
      'target_name': 'chrome_dll',
      'type': 'shared_library',
      'product_name': 'chrome',
      'include_dirs' : [
        '<@(qt_includes)',
      ],
      'defines':[
        '<@(qt_defines)',
      ],
      'sources': [
        'app/logging_chrome.cc',
        'app/logging_chrome.h',
        'app/chrome_main.cc',
        'app/chrome_main.cc',
        'app/chrome_main_delegate.cc',
        'app/chrome_main_delegate.h',
        'app/qtstatic_plugin_import.cc',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/base.gyp:base_static',
        'cr_common',
        'browser',
        'content',
      ],
      'msvs_settings': {
        'VCLinkerTool': {
          'AdditionalDependencies': [
            'imm32.lib',
            'comctl32.lib',
            '<@(qt_libs)',
          ],         
        },
      },
    },
    { #content.lib
      'target_name': 'content',
      'type': '<(component)',
      'sources': [
        'content/browser_main.cc',
        'content/browser_main.h',
        'content/browser_main_loop.cc',
        'content/browser_main_loop.h',
        'content/browser_main_parts.cc',
        'content/browser_main_parts.h',
        'content/browser_main_runner.cc',
        'content/browser_main_runner.h',
        'content/browser_process_sub_thread.cc',
        'content/browser_process_sub_thread.h',
        'content/browser_shutdown.h',
        'content/browser_thread.h',
        'content/browser_thread_delegate.h',
        'content/browser_thread_impl.cc',
        'content/browser_thread_impl.h',
        'content/content_browser_client.cc',
        'content/content_browser_client.h',
        'content/content_client.cc',
        'content/content_client.h',
        'content/content_export.h',
        'content/content_main.cc',
        'content/content_main.h',
        'content/content_main_delegate.cc',
        'content/content_main_delegate.h',
        'content/content_main_runner.cc',
        'content/content_main_runner.h',
        'content/main_function_params.h',
        'content/startup_helper_win.cc',
        'content/startup_helper_win.h',
        'content/startup_task_runner.cc',
        'content/startup_task_runner.h',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/base.gyp:base_static',
       'cr_common',
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
    { #browser.lib
      'target_name': 'browser',
      'type': 'static_library',
      'include_dirs' : [
        '<@(qt_includes)',
      ],
      'defines':[
        '<@(qt_defines)',
      ],
      'sources': [
        'browser/browser_process.cc',
        'browser/browser_process.h',
        'browser/browser_process_impl.cc',
        'browser/browser_process_impl.h',
        'browser/chrome_content_client.cc',
        'browser/chrome_content_client.h',
        'browser/chrome_content_browser_client.cc',
        'browser/chrome_content_browser_client.h',
        'browser/chrome_browser_main.cc',
        'browser/chrome_browser_main.h',
        'browser/abstract_class.cc',
        'browser/abstract_class.h',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        'cr_common',
        'content',
      ],
    },
    { #browser.lib
      'target_name': 'cr_breakpad',
      'type': 'static_library',
      'sources': [
        'breakpad/breakpad_win.cc',
        'breakpad/breakpad_win.h',
        'breakpad/crash_reporter_client.cc',
        'breakpad/crash_reporter_client.h',
        'breakpad/hard_error_handler_win.cc',
        'breakpad/hard_error_handler_win.h',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/breakpad/breakpad.gyp:breakpad_handler',        
        'cr_common',
      ],
    },
  ],
}
