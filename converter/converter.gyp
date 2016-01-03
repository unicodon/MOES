{
  'variables': {
  },
  'target_defaults': {
    'include_dirs': [
      '../format',
    ],
    'defines': ['WIN32', '_CONSOLE'],

    # Debug/Release config
    'configurations': {
      'Debug': {
        'defines': ['DEBUG', '_DEBUG'],
        'cflags': ['-g', '-O0'],
        'msbuild_settings': {
          'ClCompile' : {
            'Optimization': 'Disabled',
            'RuntimeLibrary': 'MultiThreadedDebugDLL',
          },
          'Link': {
            'GenerateDebugInformation' : 'true',
          },
        },
      },
      'Release': {
        'defines': ['NDEBUG'],
        'cflags': ['-O2'],
        'msbuild_settings': {
          'ClCompile' : {
            'Optimization': 'MaxSpeed',
            'RuntimeLibrary': 'MultiThreadedDLL',
          },
          'Link': {
            'GenerateDebugInformation' : 'false',
          },
        },
      },
    },
    'default_configuration': 'Debug',
    # MS build
    'msbuild_settings': {
      'ClCompile' : {
      },
      'Link': {
        'SubSystem': 'Console',
        'OptimizeReferences': 'true',
        'AdditionalDependencies': [ '%(AdditionalDependencies)' ],
      },
    },
    'msbuild_toolset': 'v120',
      'msbuild_configuration_attributes': {
      'CharacterSet': 'Unicode',
      'IntermediateDirectory': '$(SolutionDir)/../build/Intermediate/$(Configuration)/$(ProjectName)/',
      'OutputDirectory': '$(SolutionDir)/../build/$(Configuration)',
    },
  },
  'targets': [
    {
      'target_name': 'pmx2mom',
      'type': 'executable',
      'msvs_guid': '89BA8AFA-9AC0-45B9-8E26-5E48898294F0',
      'sources': [
        'pmx2mom/pmx2mom.cpp',
      ],
    },
    {
      'target_name': 'vmd2moa',
      'type': 'executable',
      'msvs_guid': '950741A3-6E04-4108-92CD-DEB48A8360A9',
      'sources': [
        'vmd2moa/vmd2moa.cpp',
      ],
    },
  ],
}
