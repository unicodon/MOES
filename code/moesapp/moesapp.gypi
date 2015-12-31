{
  'targets' : [
    {
      'target_name': 'moesapp',
      'type': 'executable',
      'msvs_guid': '29412EB8-2475-4427-978A-6F93D46EF0EE',
      'dependencies': [
        'JAF',
        'MOEGUL',
        'MOES',
      ],
      'link_settings': {
        'libraries': [
          '$(SolutionDir)/../build/$(Configuration)/lib/JAF<(STATIC_LIB_SUFFIX)',
          '$(SolutionDir)/../build/$(Configuration)/lib/MOEGUL<(STATIC_LIB_SUFFIX)',
          '$(SolutionDir)/../third_party/$(OES_LIB_NAME)/lib/libGLESv2<(STATIC_LIB_SUFFIX)',
          '$(SolutionDir)/../third_party/$(OES_LIB_NAME)/lib/libEGL<(STATIC_LIB_SUFFIX)',
        ],
      },
      'msbuild_settings': {
        'Link': {
          'conditions': [
            ['OS=="win"', { 'SubSystem': 'Windows' } ],
          ],
        },
      },
      'include_dirs' : [
        '../JAF/include/JAF',
        '../MOEGUL/include',
        '../MOES/include',
        '$(SolutionDir)/../third_party/glm/',
       ],
      'sources' : [
        'JAFAppMain.cpp',
        './<(OS)/PlatformExtensions.cpp',
        './<(OS)/PlatformExtensions.h',
      ],
    }
  ],
}
