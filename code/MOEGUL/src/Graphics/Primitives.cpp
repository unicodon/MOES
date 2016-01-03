//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <MOEGUL/Primitives.h>

#include <glm/gtc/type_ptr.hpp>
#include <MOEGUL/PresetShaders.h>

namespace MOEGUL
{
	namespace Primitives
	{
		void drawLine(const Camera& camera, const glm::vec3& from, const glm::vec3& to, const float color[4])
		{
			GL& gl = GL::getInstance();
			gl.enable(gl.BLEND);
			gl.enable(gl.DEPTH_TEST);

			// ArrayBuffer/IndexBufferは外しておく
			gl.bindBuffer(gl.ARRAY_BUFFER, 0);
			gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, 0);

			// シェーダ切り替え
			Program prog = PresetShaders::getPresetShader(PresetShaders::kProgramColor);
			gl.useProgram(prog);

			// Uniform設定
			static auto locColor = gl.getUniformLocation(prog, "u_color");
			static auto locMVP = gl.getUniformLocation(prog, "u_mvp");
			if (locColor < 0 || locMVP < 0) {
				return;
			}
			gl.uniform4fv(locColor, 1, color);
			glm::mat4 mvp = camera.proj() * camera.view();
			gl.uniformMatrix4fv(locMVP, 1, false, glm::value_ptr(mvp));

			// Vertex設定
			static auto locPosition = gl.getAttribLocation(prog, "a_position");
			if (locPosition < 0) {
				return;
			}
			gl.enableVertexAttribArray(locPosition);
			float vert[6] = { from[0], from[1], from[2], to[0], to[1], to[2] };
			gl.vertexAttribPointer(locPosition, 3, gl.FLOAT, false, sizeof(float) * 3, vert);

			// Draw
			gl.drawArrays(gl.LINES, 0, 2);
		}
	}
}
