//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <glm/glm.hpp>

namespace MOEGUL
{
	class Camera {
	public:
		Camera();

		glm::mat4 proj() const { return m_projection; }
		glm::mat4 view() const { return m_view; }
		glm::vec4 pos() const { return m_view[3]; }

		float fovy() const { return m_fovy; }
		float aspect() const { return m_aspect; }
	private:
		glm::mat4 m_projection;
		glm::mat4 m_view;
		float m_aspect;
		float m_fovy;
	};
}
