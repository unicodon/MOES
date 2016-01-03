//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include "Forward.h"
#include <JAFObj.h>
#include <stdio.h>

namespace MOES
{
	class ModelIF {
		MAKE_JAFOBJ(ModelIF)
	public:
		virtual ~ModelIF() {}

		//---------------------------------
		// Model API
		//---------------------------------
		/// ���݂̃t���[�����Z�b�g����
		virtual void setFrame(float frame) = 0;
		/// ���݂̃A�j���[�V�����̍ő�t���[�����擾����
		virtual float getMaxFrame() const = 0;
		/// ���݂̃t���[���œ�����Ԃ��X�V����
		virtual void update() = 0;
		/// ���f���f�[�^(MOM)��ǂݍ���
		virtual bool loadModelData(FILE* fp) = 0;
		/// ���[�V�����f�[�^(MOA)��ǂݍ���
		virtual bool loadMotionData(FILE* fp) = 0;
		/// ���f����`�悷��
		virtual void draw(const MOEGUL::Camera* camera) = 0;

		virtual void drawTest(const MOEGUL::Camera* camera) = 0;
	};
	typedef ModelIF::Ptr ModelPtr;
	ModelPtr createModel();
}
