{
  'targets' : [
    {
      'target_name': 'MOEGUL',
      'msvs_guid': 'D58C8313-3228-4EB0-9954-DE1946718D52',
      'type': 'static_library',
      'include_dirs' : [
        'include',
        'src',
        '../JAF/include/JAF',
        '$(SolutionDir)/../third_party/$(OES_LIB_NAME)/include',
        '$(SolutionDir)/../third_party/glm/',
      ],
      'sources' : [
        'include/MOEGUL/Camera.h',
        'include/MOEGUL/EGL.h',
        'include/MOEGUL/GL.h',
        'include/MOEGUL/GLTypes.h',
        'include/MOEGUL/GLUtil.h',
        'include/MOEGUL/PresetShaders.h',
        'include/MOEGUL/Primitives.h',
        'src/debug.h',
        'src/EGL/EGL.cpp',
        'src/EGL/EGL_<(OS).cpp',
        'src/EGL/EGLPrivate.h',
        'src/GL/GL.cpp',
        'src/GL/GLUtil.cpp',
        'src/Graphics/Camera.cpp',
        'src/Graphics/PresetShaders.cpp',
        'src/Graphics/Primitives.cpp',
      ],
    }
  ],
}
