//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include "Camera.h"

namespace MOEGUL
{
	namespace Primitives
	{
		void drawLine(const Camera& camera, const glm::vec3& from, const glm::vec3& to, const float color[4]);
	}
}
