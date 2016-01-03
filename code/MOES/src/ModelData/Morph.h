//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>
#include <vector>

namespace MOES
{
	/// ���_���[�t�f�[�^
	struct VertexMorphElem {
		/// ���[�t�Ώۂ̒��_Index
		int32_t m_idx;
		/// ���_�ψʗ�
		float m_offset[3];
	};

	struct VertexMorph {
		std::vector<VertexMorphElem> m_elems;
	};
}
