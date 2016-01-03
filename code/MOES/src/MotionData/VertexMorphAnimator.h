//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>
#include "MotionData.h"
#include "ModelData/ModelData.h"

namespace MOES
{
	class VertexMorphAnimator {
		MAKE_JAFOBJ(VertexMorphAnimator);
	public:
		/// MotionData‚ÆModel‚ðƒoƒCƒ“ƒh‚·‚é
		static Ptr create(const MotionData* motion, const ModelData* model);
		~VertexMorphAnimator();

		/// @param frame [in]
		/// @param transforms [out] m_vertexCount * 3 ‚Ìvector‚ÉŒ‹‰Ê‚ðŠi”[‚µ‚Ä•Ô‚·
		void getVertexTransforms(float frame, std::vector<float> &transforms);

		int32_t maxFrameNo() const { return m_maxFrameNo; }

	private:
		//-----------------------------
		// private structures
		//-----------------------------

		struct MorphFrame {
			int32_t m_frameNo;
			float m_weight;
		};
		typedef std::vector<MorphFrame> MorphFrameList;

		struct Track {
			VertexMorph m_morph;
			MorphFrameList m_frames;
		};

		//-----------------------------
		// private functions
		//-----------------------------
		VertexMorphAnimator(const MotionData* motion, const ModelData* model);

		//-----------------------------
		// private variables
		//-----------------------------
		std::vector<Track> m_tracks;

		int32_t m_vertexCount;
		int32_t m_maxFrameNo;
	};
	typedef VertexMorphAnimator::Ptr VertexMorphAnimatorPtr;
}
