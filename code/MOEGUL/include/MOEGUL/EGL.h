//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <functional>
#include <vector>
#include <JAFObj.h>

namespace MOEGUL
{
	namespace EGL
	{
		class ContextIF {
			MAKE_JAFOBJ(ContextIF);
		public:
			typedef std::unique_ptr<ContextIF> Ptr;
			virtual ~ContextIF() { };

			virtual void makeCurrent() = 0;
			virtual void swapBuffers() = 0;

			// Context”jŠü‚ÉŒÄ‚Î‚ê‚éCleanupFunction‚ğ“o˜^‚·‚é
			virtual void registerCleanupFunction(std::function<void()>) = 0;

		};

		/// Œ»İCurrent‚É‚È‚Á‚Ä‚¢‚éContext‚ğæ“¾‚·‚é
		ContextIF* getCurrentContext();

		/// CurrentContext‚ÉCleanupFunction‚ğ“o˜^‚·‚é
		bool registerCleanupFunction(std::function<void()>);
	}
	typedef EGL::ContextIF::Ptr EGLContextPtr;

	/// Native window —p‚ÌContext‚ğ¶¬‚·‚é
	EGLContextPtr createNativeEGLContext();
}
