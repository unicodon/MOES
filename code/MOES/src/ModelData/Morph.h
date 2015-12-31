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
	/// 頂点モーフデータ
	struct VertexMorphElem {
		/// モーフ対象の頂点Index
		int32_t m_idx;
		/// 頂点変位量
		float m_offset[3];
	};

	struct VertexMorph {
		std::vector<VertexMorphElem> m_elems;
	};
}
