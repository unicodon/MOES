{
  'targets' : [
    {
      'target_name': 'JAF',
      'msvs_guid': '367818CD-AADC-4C44-8F27-85DF602212B2',
      'type': 'static_library',
      'include_dirs' : [ 'include/JAF' ],
      'sources' : [
        'include/JAF/JAFFile.h',
        'include/JAF/JAFKeyboard.h',
        'include/JAF/JAFMouse.h',
        'include/JAF/JAFObj.h',
        'include/JAF/JAFPlatform.h',
        'include/JAF/JAFWindow.h',
        'include/JAF/JTL.h',
        'src/JAFWindow.cpp',
        'src/<(OS)/JAFPlatformWindow.cpp',
        'src/<(OS)/JAFPlatformWindow.h',
      ],
    }
  ],
}
