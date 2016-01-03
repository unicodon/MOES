//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdlib.h>
#include <vector>
#include <JAFObj.h>
#include <MOAData.h>

namespace MOES
{
	// モーションの元データ。
	// モデルデータとバインドしてMotionAnimatorを作る
	class MotionData {
		MAKE_JAFOBJ(MotionData);
	public:
		static Ptr create(FILE* fp);
		~MotionData();

		typedef std::vector<MOA::Name> Names;
		typedef std::vector<MOA::BoneFrame> BoneFrames;
		typedef std::vector<MOA::MorphFrame> MorphFrames;

		const BoneFrames& boneFrames() const { return m_boneFrames; }
		const Names& boneNames() const { return m_boneNames; }
		const MorphFrames& morphFrames() const { return m_morphFrames; }
		const Names& morphNames() const { return m_morphNames; }

	private:
		BoneFrames m_boneFrames;
		Names m_boneNames;
		MorphFrames m_morphFrames;
		Names m_morphNames;
	};
	typedef MotionData::Ptr MotionDataPtr;
}
