//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <MOEGUL/Camera.h>
#include <glm/gtc/matrix_transform.hpp>

namespace MOEGUL
{
	// Default camera
	Camera::Camera()
	{
		m_aspect = 8.0f / 9.0f;
		m_fovy = glm::pi<float>() * 0.25f;
		m_projection = glm::perspectiveLH(m_fovy, m_aspect, 0.1f, 100.f);
		m_view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 3.0f));
		m_view = glm::scale(m_view, glm::vec3(0.1f, 0.1f, 0.1f));
	}
}
