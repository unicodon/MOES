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
	/// �}�E�X���
	struct MouseState {
		static const uint32_t kMouseButtonLeft = 0x1U << 0;
		static const uint32_t kMouseButtonRight = 0x1U << 1;

		// �}�E�X�̈ʒu�B���C���E�B���h�E�ɑ΂��Ă̑��Βl�Ƃ��ĕԂ��B
		// �}�E�X�|�C���^���E�B���h�E��ɂ���ꍇ��[0, 1]�͈̔͂ɂȂ�
		float x; ///< �}�E�XX�ʒu
		float y; ///< �}�E�XY�ʒu

		/// ���݉�����Ă���}�E�X�{�^�����AkMouseButton* �̃r�b�g�a�ŕ\��
		uint32_t buttons;

		/// MouseState�̑��̃����o���L�����ǂ����B
		/// �}�E�X��Ԃ��擾�ł��Ȃ��ꍇ�� isValid �� false �ɂȂ�
		bool isValid;
	};

	/// ���݂̃}�E�X��Ԃ��擾����
	MouseState getMouseState();
}