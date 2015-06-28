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
  },
  'targets': [
    { #All
      'target_name': '__base__',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base_unittests',
        '<(DEPTH)/third_party/leveldatabase/leveldatabase.gyp:leveldb_db_test',
        '<(DEPTH)/mdgate/mdgate.gyp:chrome_dll',
      ],
    },
    { #crashreport.lib
      'target_name': 'crashreport',
      'type': 'static_library',
      'sources': [
        'crashreport/breakpad_win.cc',
        'crashreport/breakpad_win.h',
        'crashreport/crash_reporter_client.cc',
        'crashreport/crash_reporter_client.h',
        'crashreport/hard_error_handler_win.cc',
        'crashreport/hard_error_handler_win.h',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/breakpad/breakpad.gyp:breakpad_handler',        
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
        'app/chrome_env_vars.cc',
        'app/chrome_env_vars.h',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/base/base.gyp:base_static',
        '<(DEPTH)/breakpad/breakpad.gyp:breakpad_handler',
        '<(DEPTH)/content/content.gyp:content_static',
        'crashreport',
      ],      
      'msvs_settings': {
        'VCLinkerTool': {
          'SubSystem': '2',         # Set /SUBSYSTEM:WINDOWS
        },
      },
    }, 
  ],#targets
}
