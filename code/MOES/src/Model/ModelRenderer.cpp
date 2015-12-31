//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "ModelRenderer.h"

#include <glm/gtc/type_ptr.hpp>
#include <JTL.h>
#include <MOEGUL/EGL.h>
#include <MOEGUL/GLUtil.h>

namespace
{
	MOEGUL::Program defaultProgram()
	{
		static MOEGUL::Program s_prog;
		if (s_prog) {
			return s_prog;
		}

		const char *vs = STRINGIFY(
			//---VERTEX SHADER---
		attribute vec4 a_position;
		attribute vec3 a_normal;
		attribute vec2 a_uv;
		attribute vec4 a_bone;
		attribute vec4 a_weight;
		attribute vec3 a_morph;

		varying vec2 v_uv;
		varying vec4 v_color;
		uniform mat4 u_mvp;
		uniform vec4 u_color;
		uniform mat4 u_palette[16];


		void main()
		{
			v_uv = a_uv;
			vec4 pos = a_position;
			pos.xyz += a_morph;
			pos =
				u_palette[int(a_bone.x)] * pos * a_weight.x +
				u_palette[int(a_bone.y)] * pos * a_weight.y +
				u_palette[int(a_bone.z)] * pos * a_weight.z +
				u_palette[int(a_bone.w)] * pos * a_weight.w;

			gl_Position = u_mvp * pos;
			v_color = u_color;
//			v_color.rgb = a_weight.rgb;
		}
		//---VERTEX SHADER---
		);
		const char *fs = STRINGIFY(
			//---FRAGMENT SHADER---
			precision mediump float;
		varying vec2 v_uv;
		varying vec4 v_color;
		uniform sampler2D u_texture;
		void main()
		{
			gl_FragColor = texture2D(u_texture, v_uv) * v_color;
//			gl_FragColor = v_color;
		}
		//---FRAGMENT SHADER---
		);

		s_prog = MOEGUL::buildShaderProgram(vs, fs);

		// EGLContext破棄時にProgramも破棄
		MOEGUL::EGL::registerCleanupFunction([]() {
			s_prog.reset();
		});

		return s_prog;
	}
}

namespace MOES
{
	void renderModel(
		const MOEGUL::Camera* camera,
		const ModelData* model,
		const std::vector<glm::mat4>& boneTransforms,
		const std::vector<float>& vertexTransforms
		)
	{
		if (!camera) {
			return;
		}
		if (!model) {
			return;
		}

		auto& gl = MOEGUL::GL::getInstance();

		// 初期設定
		gl.enable(gl.DEPTH_TEST);
		gl.enable(gl.BLEND);
		gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
		gl.enable(gl.CULL_FACE);
		gl.frontFace(gl.CW);
		gl.cullFace(gl.BACK);

		auto prog = defaultProgram();
		gl.useProgram(prog);
		// AttribLocation を取得
		static auto locPosition = gl.getAttribLocation(prog, "a_position");
		static auto locNormal = gl.getAttribLocation(prog, "a_normal");
		static auto locUV = gl.getAttribLocation(prog, "a_uv");
		static auto locBone = gl.getAttribLocation(prog, "a_bone");
		static auto locWeight = gl.getAttribLocation(prog, "a_weight");
		static auto locMorph = gl.getAttribLocation(prog, "a_morph");

		if (locPosition >= 0) { gl.enableVertexAttribArray(locPosition); }
		if (locNormal >= 0) { gl.enableVertexAttribArray(locNormal); }
		if (locUV >= 0) { gl.enableVertexAttribArray(locUV); }
		if (locBone >= 0) { gl.enableVertexAttribArray(locBone); }
		if (locWeight >= 0) { gl.enableVertexAttribArray(locWeight); }
		if (locMorph >= 0) { gl.enableVertexAttribArray(locMorph); }

		// UniformLocation を取得
		static auto locColor = gl.getUniformLocation(prog, "u_color");
		static auto locMVP = gl.getUniformLocation(prog, "u_mvp");
		static auto locTexture = gl.getUniformLocation(prog, "u_texture");
		static auto locPalette = gl.getUniformLocation(prog, "u_palette");

		// カメラ設定
		glm::mat4 mvp = camera->proj() * camera->view();
		gl.uniformMatrix4fv(locMVP, 1, false, glm::value_ptr(mvp));
		float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		gl.uniform4fv(locColor, 1, color);
		gl.uniform1i(locTexture, 0);

		// テクスチャスロット設定
		gl.uniform1i(locTexture, 0);

		const auto& meshes = model->meshes();
		for (const auto& mesh : meshes) {
			// Morph データを作成
			std::vector<float> morphData;
			int32_t morphSize = mesh.m_originalVertexIndices.size();
			morphData.resize(morphSize * 3);
			memset(&morphData[0], 0, sizeof(float) * morphSize * 3);
			for (int i = 0; i < morphSize; i++) {
				int32_t originalIdx = mesh.m_originalVertexIndices[i];
				if (originalIdx * 3 < vertexTransforms.size()) {
					memcpy(&morphData[i * 3], &vertexTransforms[originalIdx * 3], sizeof(float) * 3);
				}
			}

			// Texture をセット
			gl.bindTexture(gl.TEXTURE_2D, mesh.m_tex);

			// Palette をセット
			glm::mat4 palette[Mesh::kMaxMatrices];
			for (int i = 0; i < Mesh::kMaxMatrices; i++) {
				int32_t bone = mesh.m_bones[i];
				if (bone < boneTransforms.size()) {
					palette[i] = boneTransforms[bone];
				}
			}
			gl.uniformMatrix4fv(locPalette, Mesh::kMaxMatrices, false, (const float*)palette);

			// VBOをセット
			auto vbo = mesh.m_vbo;
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo);

			// Attrib pointer を設定
			gl.vertexAttribPointer(locPosition, 3, gl.FLOAT, false, sizeof(Vertex), (uint8_t*)0 + offsetof(Vertex, m_pos));
//			gl.vertexAttribPointer(locNormal, 3, gl.FLOAT, false, sizeof(Vertex), (uint8_t*)0 + offsetof(Vertex, m_normal));
			gl.vertexAttribPointer(locUV, 2, gl.FLOAT, false, sizeof(Vertex), (uint8_t*)0 + offsetof(Vertex, m_uv));
			gl.vertexAttribPointer(locBone, 4, gl.FLOAT, false, sizeof(Vertex), (uint8_t*)0 + offsetof(Vertex, m_bone));
			gl.vertexAttribPointer(locWeight, 4, gl.FLOAT, false, sizeof(Vertex), (uint8_t*)0 + offsetof(Vertex, m_weight));

			gl.bindBuffer(gl.ARRAY_BUFFER, 0);
			gl.vertexAttribPointer(locMorph, 3, gl.FLOAT, false, 0, &morphData[0]);

			// IBOをセットして描画
			auto ibo = mesh.m_ibo;
			gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ibo);
			gl.drawElements(gl.TRIANGLES, mesh.m_indexCount, gl.UNSIGNED_INT, (int32_t*)(0));
		}
	}
}
