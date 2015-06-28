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
  'includes': [
    'qt_toolset.gypi',
    'ctpapi.gypi'
  ],
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
        '<(DEPTH)/content/content.gyp:content_static',
        'chrome_dll',
        'crashreport',
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
        'common/chrome_env_vars.cc',
        'common/chrome_env_vars.h',
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
        '<(DEPTH)/content/content.gyp:content',
        '<(DEPTH)/content/content.gyp:content_static',
        'cr_common',
        'browser',
      ],
      'msvs_settings': {
        'VCLinkerTool': {
          'AdditionalDependencies': [
            'imm32.lib',
            'comctl32.lib',
            '<@(qt_libs)',
            '<@(ctpapi_libs)',
          ],         
        },
      },
    },
    { #browser.lib
      'target_name': 'browser',
      'type': 'static_library',
      'include_dirs' : [
        '<@(qt_includes)',
        '<@(ctpapi_includes)',
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
        
        # ctp
        'browser/ctp/mdapi.cc',
        'browser/ctp/mdapi.h',
        'browser/ctp/tdapi.cc',
        'browser/ctp/tdapi.h',
                
        # browser ui
        'browser/ui/messageloop_qt.cc',
        'browser/ui/messageloop_qt.h',
        'browser/ui/main_window.cc',
        'browser/ui/main_window.h',
        'browser/ui/trading_widget.cc',
        'browser/ui/trading_widget.h',
        'browser/ui/marketdata_widget.cc',
        'browser/ui/marketdata_widget.h',
        'browser/ui/order_widget.cc',
        'browser/ui/order_widget.h',
        'browser/ui/trade_widget.cc',
        'browser/ui/trade_widget.h',
        'browser/ui/log_widget.cc',
        'browser/ui/log_widget.h',
        'browser/ui/position_widget.cc',
        'browser/ui/position_widget.h',
        'browser/ui/account_widget.cc',
        'browser/ui/account_widget.h',
        
        # Generated MOC files
        '<(moc_gen_dir)/moc_messageloop_qt.cc',        
        '<(moc_gen_dir)/moc_main_window.cc',
        '<(moc_gen_dir)/moc_trading_widget.cc',
        '<(moc_gen_dir)/moc_marketdata_widget.cc',
        '<(moc_gen_dir)/moc_order_widget.cc',
        '<(moc_gen_dir)/moc_trade_widget.cc',
        '<(moc_gen_dir)/moc_log_widget.cc',
        '<(moc_gen_dir)/moc_account_widget.cc',
        '<(moc_gen_dir)/moc_position_widget.cc',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/content/content.gyp:content',
        'cr_common',
        'browser_qt_mocs',
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
        'cr_common',
      ],
    },
    { #browser_qt_mocs
      'target_name': 'browser_qt_mocs',
      'type': 'none',
      'sources': [
        '<(moc_src_dir)/messageloop_qt.h',
        '<(moc_src_dir)/main_window.h',
        '<(moc_src_dir)/trading_widget.h',
        '<(moc_src_dir)/marketdata_widget.h',
        '<(moc_src_dir)/order_widget.h',
        '<(moc_src_dir)/trade_widget.h',
        '<(moc_src_dir)/log_widget.h',
        '<(moc_src_dir)/account_widget.h',
        '<(moc_src_dir)/position_widget.h',
      ],
      'rules': [
        {
          'rule_name': 'generate_moc',
          'extension': 'h',
          'outputs': [ '<(moc_gen_dir)/moc_<(RULE_INPUT_ROOT).cc' ],
          'action': [ '<(qt_moc)', '-DXXX',
                      '<(RULE_INPUT_PATH)',
                      '-o', '<(moc_gen_dir)/moc_<(RULE_INPUT_ROOT).cc' ],
          'message': 'Generating <(RULE_INPUT_ROOT).cc.',
        },
      ],
    },    
  ],
}
