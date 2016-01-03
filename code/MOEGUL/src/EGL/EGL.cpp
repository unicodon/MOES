//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "EGLPrivate.h"

namespace MOEGUL
{
	namespace EGL
	{
		// FIXME: thread_local ÇégÇ¢ÇΩÇ¢
		__declspec(thread) static Context* s_current = nullptr;

		void Context::makeCurrent()
		{
			eglMakeCurrent(m_display, m_surface, m_surface, m_context);
			SHOW_EGL_ERROR();
			s_current = this;
		}

		void Context::swapBuffers()
		{
			eglSwapBuffers(m_display, m_surface);
			SHOW_EGL_ERROR();
		}

		void Context::registerCleanupFunction(std::function<void()> func)
		{
			m_cleanups.push_back(func);
		}

		void Context::callCleanupFunctions()
		{
			makeCurrent();
			for (auto func : m_cleanups)
			{
				func();
			}
			s_current = nullptr;
		}

		ContextIF* getCurrentContext()
		{
			return s_current;
		}

		bool registerCleanupFunction(std::function<void()> func)
		{
			ContextIF* context = getCurrentContext();
			if (context) {
				context->registerCleanupFunction(func);
				return true;
			}
			return false;
		}
		
		ContextIF::ContextIF()
		{
		}
	}

	/// Native window ópÇÃContextÇê∂ê¨Ç∑ÇÈ
	EGLContextPtr createNativeEGLContext()
	{
		return EGL::Context::create();
	}
}
