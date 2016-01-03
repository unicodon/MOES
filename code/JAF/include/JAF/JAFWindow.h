//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include "JAFPlatform.h"

#include <functional>

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
		virtual PlatformWindowHandle platformWindowHandle() const = 0;
		virtual PlatformDisplay platformDisplay() const = 0;
		virtual bool update() = 0;
		//virtual bool showErrorDialog(const char* msg) = 0;

		/// Window �ɑ΂��ăt�@�C��Open�v�����������ꍇ�ɁA�t�@�C���p�X��
		/// UTF-8 ������Œʒm���邽�߂̃R�[���o�b�N
		typedef std::function< void(const char*)> FileOpenCallback;

		virtual void registerFileOpenCallback(const FileOpenCallback&) = 0;

	protected:
		Window();
		virtual ~Window();
	};
}
