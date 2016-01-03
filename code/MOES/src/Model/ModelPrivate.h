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
		/// 現在のフレームをセットする
		virtual void setFrame(float frame);
		/// 現在のアニメーションの最大フレームを取得する
		virtual float getMaxFrame() const;
		/// 現在のフレームで内部状態を更新する
		virtual void update();
		/// モデルデータを読み込む
		virtual bool loadModelData(FILE* fp);
		/// モーションデータ(MOA)を読み込む
		virtual bool loadMotionData(FILE* fp);
		/// モデルを描画する
		virtual void draw(const MOEGUL::Camera* camera);

		virtual void drawTest(const MOEGUL::Camera* camera);

	private:
		void onUpdateModelData();

		// 現在のフレーム
		float m_frame;

		ModelDataPtr m_model;
		MotionDataPtr m_motion;
		BoneAnimatorPtr m_boneAnimator;
		VertexMorphAnimatorPtr m_vertexMorphAnimator;

		std::vector<glm::mat4> m_boneTransforms;
		std::vector<float> m_vertexTransforms;
	};
}