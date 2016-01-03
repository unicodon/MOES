{
  'default_configuration': 'Debug',
  
  'target_defaults': {  
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
            'OptimizeReferences' : 'true',
          },
        },
      },
    },
    # common
    'msbuild_settings': {
      'Link': {
        'AdditionalDependencies': [ '%(AdditionalDependencies)' ],
      },
    },
    'msbuild_toolset': 'v120',
    'msbuild_configuration_attributes': {
      'CharacterSet': 'Unicode',
      'IntermediateDirectory': '$(SolutionDir)/../build/Intermediate/$(Configuration)/$(ProjectName)/',
      'OutputDirectory': '$(SolutionDir)/../build/$(Configuration)',
    },
    # OS props
    'msbuild_props': [
      '<(OS).props'
    ],
  },
}
