//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>
#include <MOEGUL/GL.h>

#include "Material.h"
#include "Vertex.h"

namespace MOES
{
	struct Mesh {
		static const int32_t kMaxMatrices = 16;

		int32_t m_indexCount;
		MOEGUL::Buffer m_vbo;
		MOEGUL::Buffer m_ibo;
		MOEGUL::Texture m_tex;
		Material m_material;
		// パレットの参照先ボーン
		int32_t m_bones[kMaxMatrices];
		// 元の(サブメッシュ分割前の)VertexIndex
		std::vector<int32_t> m_originalVertexIndices;
	};
}
