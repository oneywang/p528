{
  'variables': {
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
     'qt_moc%': '<(qt_sdk)/bin/moc',
     'moc_src_dir': '<(DEPTH)/chrome/browser/ui',
     'moc_gen_dir': '<(SHARED_INTERMEDIATE_DIR)/chrome/browser/ui',
  },
}