//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

namespace MOES
{
	struct Material {
		int32_t m_textureIdx;
		float m_diffuse[4];
		float m_specular[4];
		float m_ambient[4];
	};
}
