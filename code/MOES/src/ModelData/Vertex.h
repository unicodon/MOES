//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>

namespace MOES
{
	struct Vertex {
		float m_pos[3];
		float m_normal[3];
		float m_uv[2];
		float m_bone[4];
		float m_weight[4];
	};
}
