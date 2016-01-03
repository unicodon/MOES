//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <MOES/Model.h>
#include "ModelData/ModelData.h"
#include "MotionData/MotionData.h"
#include "MotionData/BoneAnimator.h"
#include "MotionData/VertexMorphAnimator.h"

namespace MOES
{
	class Model : public ModelIF {
	public:
		Model();
		~Model();

		//---------------------------------
		// Model API
		//---------------------------------
		/// ���݂̃t���[�����Z�b�g����
		virtual void setFrame(float frame);
		/// ���݂̃A�j���[�V�����̍ő�t���[�����擾����
		virtual float getMaxFrame() const;
		/// ���݂̃t���[���œ�����Ԃ��X�V����
		virtual void update();
		/// ���f���f�[�^��ǂݍ���
		virtual bool loadModelData(FILE* fp);
		/// ���[�V�����f�[�^(MOA)��ǂݍ���
		virtual bool loadMotionData(FILE* fp);
		/// ���f����`�悷��
		virtual void draw(const MOEGUL::Camera* camera);

		virtual void drawTest(const MOEGUL::Camera* camera);

	private:
		void onUpdateModelData();

		// ���݂̃t���[��
		float m_frame;

		ModelDataPtr m_model;
		MotionDataPtr m_motion;
		BoneAnimatorPtr m_boneAnimator;
		VertexMorphAnimatorPtr m_vertexMorphAnimator;

		std::vector<glm::mat4> m_boneTransforms;
		std::vector<float> m_vertexTransforms;
	};
}