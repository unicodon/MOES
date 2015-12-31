//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include <MOEGUL/EGL.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <JTL.h>
#include <stdio.h>

#include "debug.h"

#define SHOW_EGL_ERROR() do { \
	EGLint e = eglGetError(); if (e != EGL_SUCCESS) { DBG_ERROR("EGLError %X\n", e); } \
} while (0)

namespace MOEGUL
{
	namespace EGL
	{
		class Context : public ContextIF {
		public:
			/// Native window ópÇÃContextÇê∂ê¨Ç∑ÇÈ
			static Ptr create();
			Context();
			virtual ~Context();

			virtual void makeCurrent();
			virtual void swapBuffers();

			// Contextîjä¸éûÇ…åƒÇŒÇÍÇÈCleanupFunctionÇìoò^Ç∑ÇÈ
			virtual void registerCleanupFunction(std::function<void()>);

		private:
			void callCleanupFunctions();

		private:
			EGLDisplay m_display;
			EGLSurface m_surface;
			EGLContext m_context;

			std::vector<std::function<void()> > m_cleanups;
		};
	}
}
