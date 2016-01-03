//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>

namespace JAF
{
	/// ����̃L�[�ɂ��Ă̌��݂̉������
	struct KeyboardState {
		enum {
			kKeyGroup0 = 0,
			kKeyGroupNum
		};
		enum KeyCode {
			// Group 0
			kKeyCodeEnter	= kKeyGroup0 | (0x1U << 0),
			kKeyCodeEsc		= kKeyGroup0 | (0x1U << 1),

			kKeyCodeLeft	= kKeyGroup0 | (0x1U << 4),
			kKeyCodeRight	= kKeyGroup0 | (0x1U << 5),
			kKeyCodeUp		= kKeyGroup0 | (0x1U << 6),
			kKeyCodeDown	= kKeyGroup0 | (0x1U << 7),
		};

		uint32_t keys[kKeyGroupNum];

		/// KeyboardState�̑��̃����o���L�����ǂ����B
		bool isValid;
	};

	/// ���݂̃L�[�{�[�h��Ԃ��擾����
	KeyboardState getKeyboardState();
}
