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
        'defines': ['_DEBUG'],
        'msbuild_settings': {
          'ClCompile' : {
            'Optimization': 'Disabled',
            'RuntimeLibrary': 'MultiThreadedDebugDLL',
         },
        },
      },
      'Release': {
        'defines': ['NDEBUG'],
        'msbuild_settings': {
          'ClCompile' : {
            'Optimization': 'MaxSpeed',
            'RuntimeLibrary': 'MultiThreadedDLL',
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
      'IntermediateDirectory': '$(SolutionDir)\\$(ProjectName)\\$(Configuration)\\',
      'OutputDirectory': '$(SolutionDir)\\..\\out\\bin\\',
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
