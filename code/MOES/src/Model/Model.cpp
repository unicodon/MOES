//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "ModelPrivate.h"

#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <MOEGUL/Camera.h>
#include <MOEGUL/PresetShaders.h>
#include "ModelRenderer.h"

namespace MOES
{
	//---------------------------------
	// Model API
	//---------------------------------
	/// 現在のフレームをセットする
	void Model::setFrame(float frame)
	{
		if (frame < 0.0f) {
			m_frame = 0.0f;
		} else {
			m_frame = std::min(frame, getMaxFrame());
		}
	}

	/// 現在のアニメーションの最大フレームを取得する
	float Model::getMaxFrame() const
	{
		float frame = 0.0f;
		if (m_boneAnimator) {
			frame = std::max((float)m_boneAnimator->maxFrameNo(), frame);
		}
		if (m_vertexMorphAnimator) {
			frame = std::max((float)m_vertexMorphAnimator->maxFrameNo(), frame);
		}
		return frame;
	}

	/// モデルデータを読み込む
	bool Model::loadModelData(FILE* fp)
	{
		ModelDataPtr model = ModelData::create(fp);
		if (model) {
			m_model.swap(model);
			if (m_motion) {
				m_boneAnimator = BoneAnimator::create(m_motion.get(), m_model.get());
				m_vertexMorphAnimator = VertexMorphAnimator::create(m_motion.get(), m_model.get());
			}
			else {
				m_boneAnimator.reset();
				m_vertexMorphAnimator.reset();
			}
			onUpdateModelData();
			return true;
		}
		return false;
	}

	/// モーションデータ(MOA)を読み込む
	bool Model::loadMotionData(FILE* fp)
	{
		MotionDataPtr motion = MotionData::create(fp);
		if (motion) {
			m_motion.swap(motion);
			if (m_model) {
				m_boneAnimator = BoneAnimator::create(m_motion.get(), m_model.get());
				m_vertexMorphAnimator = VertexMorphAnimator::create(m_motion.get(), m_model.get());
			}
			else {
				m_boneAnimator.reset();
				m_vertexMorphAnimator.reset();
			}
			return true;
		}
		return false;
	}

	void Model::draw(const MOEGUL::Camera* camera)
	{
		renderModel(camera, m_model.get(), m_boneTransforms, m_vertexTransforms);
	}

	void Model::drawTest(const MOEGUL::Camera* camera)
	{
		if (!camera) {
			return;
		}
		if (!m_model) {
			return;
		}
		auto& gl = MOEGUL::GL::getInstance();

		gl.enable(gl.DEPTH_TEST);
		gl.enable(gl.BLEND);
		gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
		gl.enable(gl.CULL_FACE);
		gl.frontFace(gl.CW);
		gl.cullFace(gl.BACK);

		auto prog = MOEGUL::PresetShaders::getPresetShader(MOEGUL::PresetShaders::kProgramColorTexture);
		static auto locColor = gl.getUniformLocation(prog, "u_color");
		static auto locMVP = gl.getUniformLocation(prog, "u_mvp");
		static auto locTexture = gl.getUniformLocation(prog, "u_texture");
		static auto locPosition = gl.getAttribLocation(prog, "a_position");
		static auto locUV = gl.getAttribLocation(prog, "a_uv");

		gl.useProgram(prog);		
		glm::mat4 mvp = camera->proj() * camera->view();
		gl.uniformMatrix4fv(locMVP, 1, false, glm::value_ptr(mvp));
		float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		gl.uniform4fv(locColor, 1, color);
		gl.uniform1i(locTexture, 0);

		gl.enableVertexAttribArray(locPosition);
		// Draw
		const auto& meshes = m_model->meshes();
		const auto& textures = m_model->textures();
		for (const auto& mesh : meshes) {
			MOEGUL::Texture tex;
			if (mesh.m_material.m_textureIdx >= 0 && mesh.m_material.m_textureIdx < textures.size()) {
				tex = textures[mesh.m_material.m_textureIdx];
			}
			if (tex) {
				//::glActiveTexture(GL_TEXTURE0);
				gl.bindTexture(gl.TEXTURE_2D, tex);
				gl.uniform1i(locTexture, 0);
			}
			else {
				gl.bindTexture(gl.TEXTURE_2D, 0);
			}

			auto vbo = mesh.m_vbo;
			auto ibo = mesh.m_ibo;
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
			gl.vertexAttribPointer(locPosition, 3, gl.FLOAT, false, sizeof(Vertex), (uint8_t*)0 + offsetof(Vertex, m_pos));

			gl.enableVertexAttribArray(locUV);
			gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
			gl.vertexAttribPointer(locUV, 2, gl.FLOAT, false, sizeof(Vertex), (uint8_t*)0 + offsetof(Vertex, m_uv));

			gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ibo);

			gl.drawElements(gl.TRIANGLES, mesh.m_indexCount, gl.UNSIGNED_INT, (int32_t*)(0));
		}
	}

	//---------------------------------
	// Model データ更新処理
	//---------------------------------
	void Model::update()
	{
		if (m_boneAnimator) {
			m_boneAnimator->getBoneTransforms(m_frame, m_boneTransforms);
		}
		if (m_vertexMorphAnimator) {
			m_vertexMorphAnimator->getVertexTransforms(m_frame, m_vertexTransforms);
		}

		auto& gl = MOEGUL::GL::getInstance();

	}

	void Model::onUpdateModelData()
	{
	}

	//---------------------------------
	// Model Ctor/Dtor
	//---------------------------------
	Model::Model()
		: m_frame(0.0f)
	{
	}

	Model::~Model()
	{
	}

	ModelIF::ModelIF()
	{
	}

	/// インスタンス生成
	ModelPtr createModel()
	{
		return ModelPtr(new Model());
	}

}
