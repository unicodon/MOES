//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "EGLPrivate.h"

#include <JAFWindow.h>

namespace 
{
	EGLDisplay getPlatformDisplay()
	{
		static EGLDisplay s_display = EGL_NO_DISPLAY;
		if (s_display != EGL_NO_DISPLAY) {
			return s_display;
		}

		JAF::Window* window = JAF::Window::getMainWindow();
		if (!window) {
			return EGL_NO_DISPLAY;
		}

		PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
		if (!eglGetPlatformDisplayEXT)
		{
			return EGL_NO_DISPLAY;
		}

		EGLint attr[] = {
			EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
			EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, EGL_DONT_CARE,
			EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, EGL_DONT_CARE,
			EGL_NONE
		};

		s_display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, window->platformDisplay(), attr);
		if (s_display == EGL_NO_DISPLAY) {
			return EGL_NO_DISPLAY;
		}

		EGLint majorVersion, minorVersion;
		if (eglInitialize(s_display, &majorVersion, &minorVersion) == EGL_FALSE) {
			s_display = EGL_NO_DISPLAY;
			return EGL_NO_DISPLAY;
		}

		eglBindAPI(EGL_OPENGL_ES_API);
		if (eglGetError() != EGL_SUCCESS)
		{
			eglTerminate(s_display);
			s_display = EGL_NO_DISPLAY;
			return EGL_NO_DISPLAY;
		}

		return s_display;
	}

}

namespace MOEGUL
{
	namespace EGL
	{
		Context::Context()
			: m_display(EGL_NO_DISPLAY)
			, m_surface(EGL_NO_SURFACE)
			, m_context(EGL_NO_CONTEXT)
		{
		}

		Context::~Context()
		{
			if (m_context != EGL_NO_CONTEXT)
			{
				callCleanupFunctions();
				eglDestroyContext(m_display, m_context);
			}
			if (m_surface != EGL_NO_SURFACE)
			{
				eglDestroySurface(m_display, m_surface);
			}
		}

		/// Native window ópÇÃContextÇê∂ê¨Ç∑ÇÈ
		ContextIF::Ptr Context::create()
		{
			// Get main window
			JAF::Window* window = JAF::Window::getMainWindow();
			if (!window) {
				return Ptr();
			}

			EGLDisplay display;
			EGLSurface surface = EGL_NO_SURFACE;
			EGLContext context = EGL_NO_CONTEXT;

			// Get display
			display = getPlatformDisplay();

			SCOPE_EXIT({
				if (context != EGL_NO_CONTEXT) { eglDestroyContext(display, context); }
				if (surface != EGL_NO_SURFACE) { eglDestroySurface(display, surface); }
			});

			// Choose config
			const EGLint configAttributes[] =
			{
				EGL_RED_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_BLUE_SIZE, 8,
				EGL_ALPHA_SIZE, 8,
				EGL_DEPTH_SIZE, 24,
				EGL_STENCIL_SIZE, 8,
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				EGL_NONE
			};
			EGLint configCount;
			EGLConfig config;
			if (!eglChooseConfig(display, configAttributes, &config, 1, &configCount) || (configCount != 1))
			{
				SHOW_EGL_ERROR();
				return Ptr();
			}

			EGLint attr;
			for (int i = 0; i < sizeof(configAttributes) / sizeof(configAttributes[0]); i += 2) {
				eglGetConfigAttrib(display, config, configAttributes[i], &attr);
			}

			// Create surface
			const EGLint surfaceAttributes[] = {
				EGL_NONE
			};
			surface = eglCreateWindowSurface(display, config, window->platformWindowHandle(), surfaceAttributes);
			if (surface == EGL_NO_SURFACE)
			{
				SHOW_EGL_ERROR();
				return Ptr();
			}

			// Create context
			const EGLint contextAttributes[] = {
				EGL_CONTEXT_MAJOR_VERSION_KHR, 2,
				EGL_CONTEXT_MINOR_VERSION_KHR, 0,
				EGL_NONE
			};
			context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
			if (context == EGL_NO_CONTEXT)
			{
				SHOW_EGL_ERROR();
				return Ptr();
			}

			// Create instance
			Context* ptr = new Context;
			if (!ptr) {
				return Ptr();
			}
			ptr->m_display = display;
			ptr->m_surface = surface;
			ptr->m_context = context;
			surface = EGL_NO_SURFACE;
			context = EGL_NO_CONTEXT;
			return Ptr(ptr);
		}
	}
}
