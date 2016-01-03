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

			// Context�j�����ɌĂ΂��CleanupFunction��o�^����
			virtual void registerCleanupFunction(std::function<void()>) = 0;

		};

		/// ����Current�ɂȂ��Ă���Context���擾����
		ContextIF* getCurrentContext();

		/// CurrentContext��CleanupFunction��o�^����
		bool registerCleanupFunction(std::function<void()>);
	}
	typedef EGL::ContextIF::Ptr EGLContextPtr;

	/// Native window �p��Context�𐶐�����
	EGLContextPtr createNativeEGLContext();
}
