//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include "JAFPlatform.h"

/// JAF App �̃G���g���|�C���g�B�A�v���P�[�V�������ł��̊֐�����������B
extern "C" int JAFAppMain(int argc, char* argv[]);

namespace JAF
{
	class Window
	{
	public:
		/// �t���[�����[�N���p�ӂ��郁�C���E�B���h�E���擾����B
		static Window* getMainWindow();

		//-------------------------------------
		// JAF Window I/F
		// PlatformWindow�͈ȉ���I/F����������
		//-------------------------------------
		virtual PlatformDisplay platformDisplay() const = 0;
		virtual bool update() = 0;
		//virtual bool showErrorDialog(const char* msg) = 0;

	protected:
		Window();
		virtual ~Window();
	};
}
