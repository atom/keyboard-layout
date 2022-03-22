{
  "variables": {
    "openssl_fips" : "0" 
  },
  "targets": [
    {
      "target_name": "keyboard-layout-manager",
      "sources": [
        "src/keyboard-layout-manager.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "chrome_headers",
      ],
      "conditions": [
        ['OS=="mac"', {
          "sources": [
            "src/keyboard-layout-manager-mac.mm",
          ],
          "link_settings": {
            "libraries": [
              "-framework", "AppKit"
            ]
          }
        }],  # OS=="mac"
        ['OS=="win"', {
          "sources": [
            "src/keyboard-layout-manager-windows.cc",
          ],
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1, # /EHsc
              'WarnAsError': 'true',
            },
          },
          'msvs_disabled_warnings': [
            4309,  # 'static_cast': truncation of constant value
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
            "src/keyboard-layout-manager-linux.cc",
          ],
          "link_settings": {
            "libraries": [
              "-lX11", "-lxkbfile"
            ]
          }
        }],  # OS=="linux"
        ['OS=="freebsd"', {
          "sources": [
            "src/keyboard-layout-manager-linux.cc",
          ],
          "include_dirs": [
            "/usr/local/include", "/usr/local/include/X11",
          ],
          "ldflags": [
            "-lX11", "-lxkbfile", "-L/usr/local/lib",
          ],
        }],  # OS=="posix"
      ]
    }
  ]
}
