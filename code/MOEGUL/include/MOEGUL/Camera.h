//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

		//-------------------------------------
		// カメラ操作関数
		//-------------------------------------
		void pan(float rad)
		{
			m_view = glm::rotate(glm::mat4(1.0f), rad, glm::vec3(0.0f, 1.0f, 0.0f)) * m_view;
		}

		void tilt(float rad)
		{
			m_view = glm::rotate(glm::mat4(1.0f), rad, glm::vec3(1.0f, 0.0f, 0.0f)) * m_view;
		}

		void rotateY(float rad)
		{
			m_view = glm::rotate(m_view, rad, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		void move(float x, float y, float z)
		{
			glm::vec3 offs = glm::inverse(glm::mat3(m_view)) * glm::vec3(x, y, z);
			m_view = glm::translate(m_view, offs);
		}
	private:
		glm::mat4 m_projection;
		glm::mat4 m_view;
		float m_aspect;
		float m_fovy;
	};
}
