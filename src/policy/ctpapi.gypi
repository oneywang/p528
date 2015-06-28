{
  'variables': {
    'ctpapi_sdk': 'c:/ctpapi',    
    'ctpapi_libs': [
      '<(ctpapi_sdk)/lib/thostmduserapi.lib',    
      '<(ctpapi_sdk)/lib/thosttraderapi.lib',    
    ],
    'ctpapi_includes': [
       '<(ctpapi_sdk)/include',
     ],
  },
}