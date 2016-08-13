{
  "targets": [
    {
      "target_name": "keyboard-layout-observer",
      "include_dirs": [ "<!(node -e \"require('nan')\")" ],
      "conditions": [
        ['OS=="mac"', {
          "sources": [
            "src/keyboard-layout-observer-mac.mm",
          ],
          "link_settings": {
            "libraries": [
              "-framework", "AppKit"
            ]
          }
        }],  # OS=="mac"
        ['OS=="win"', {
          "sources": [
            "src/keyboard-layout-observer-windows.cc",
          ],
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1, # /EHsc
              'WarnAsError': 'true',
            },
          },
          'msvs_disabled_warnings': [
            4018,  # signed/unsigned mismatch
            4244,  # conversion from 'type1' to 'type2', possible loss of data
            4267,  # conversion from 'size_t' to 'type', possible loss of data
            4302,  # 'type cast': truncation from 'HKL' to 'UINT'
            4311,  # 'type cast': pointer truncation from 'HKL' to 'UINT'
            4530,  # C++ exception handler used, but unwind semantics are not enabled
            4506,  # no definition for inline function
            4577,  # 'noexcept' used with no exception handling mode specified
            4996,  # function was declared deprecated
          ],
        }],  # OS=="win"
        ['OS=="linux"', {
          "sources": [
            "src/keyboard-layout-observer-non-mac.cc",
          ],
        }],  # OS=="linux"
      ]
    }
  ]
}
