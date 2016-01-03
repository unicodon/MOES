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
		/// 現在のフレームをセットする
		virtual void setFrame(float frame) = 0;
		/// 現在のアニメーションの最大フレームを取得する
		virtual float getMaxFrame() const = 0;
		/// 現在のフレームで内部状態を更新する
		virtual void update() = 0;
		/// モデルデータ(MOM)を読み込む
		virtual bool loadModelData(FILE* fp) = 0;
		/// モーションデータ(MOA)を読み込む
		virtual bool loadMotionData(FILE* fp) = 0;
		/// モデルを描画する
		virtual void draw(const MOEGUL::Camera* camera) = 0;

		virtual void drawTest(const MOEGUL::Camera* camera) = 0;
	};
	typedef ModelIF::Ptr ModelPtr;
	ModelPtr createModel();
}
