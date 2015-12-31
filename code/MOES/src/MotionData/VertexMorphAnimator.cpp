//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "VertexMorphAnimator.h"

#include <algorithm>
#include <functional>
#include <glm/glm.hpp>

namespace MOES
{
	VertexMorphAnimator::~VertexMorphAnimator()
	{
	}

	VertexMorphAnimator::VertexMorphAnimator(const MotionData* motion, const ModelData* model)
	{
		m_vertexCount = model->vertexCount();
		m_maxFrameNo = 0;

		// MotionのMorphIDとModelのMorphIDをバインドする
		std::map<int32_t, int32_t> modelMorphIds;
		std::map<int32_t, int32_t> trackIds;
		auto morphNames = motion->morphNames();
		for (auto &morphName : morphNames) {
			const char* name = morphName.m_name;
			uint32_t id = morphName.m_id;
			int32_t resolvedId = model->vertexMorphNames().lookUp(name);
			modelMorphIds[id] = resolvedId;
			if (resolvedId < 0) {
				printf("Missing morph %s\n", name);
				continue;
			}

			// MorphData をTrackにコピー
			trackIds[id] = m_tracks.size();
			m_tracks.push_back(Track());
			m_tracks.back().m_morph = model->vertexMorphs()[resolvedId];
		}

		// Frameを取り込む
		auto morphFrames = motion->morphFrames();
		for (auto &morphFrame : morphFrames) {
			int32_t morphId = morphFrame.m_morphId;
			auto it = trackIds.find(morphId);
			if (it == trackIds.end()) {
				// トラックに未登録のMorphFrameなのでスキップ
				continue;
			}
			int32_t trackId = it->second;

			m_maxFrameNo = std::max<int32_t>(m_maxFrameNo, morphFrame.m_frameNo);

			MorphFrame frame;
			frame.m_frameNo = morphFrame.m_frameNo;
			frame.m_weight = morphFrame.m_weight;
			m_tracks[trackId].m_frames.push_back(frame);
		}

		// FrameNoでソート
		for (auto& elem : m_tracks) {
			std::sort(elem.m_frames.begin(), elem.m_frames.end(),
				[](const MorphFrame& l, const MorphFrame& r) { return l.m_frameNo < r.m_frameNo; }
			);
		}
	}

	VertexMorphAnimator::Ptr VertexMorphAnimator::create(const MotionData* motion, const ModelData* model)
	{
		VertexMorphAnimator* ptr = new VertexMorphAnimator(motion, model);
		return Ptr(ptr);
	}

	void VertexMorphAnimator::getVertexTransforms(float frame, std::vector<float> &transforms)
	{
		// 0 埋め
		if (transforms.size() < 3 * m_vertexCount) {
			transforms.resize(3 * m_vertexCount);
		}
		memset(&transforms[0], 0, sizeof(float) * 3 * m_vertexCount);

		size_t count = m_tracks.size();
		for (size_t i = 0; i < count; i++) {
			float weight = 0.0f;
			auto &inf = std::find_if(m_tracks[i].m_frames.begin(), m_tracks[i].m_frames.end(), [frame](const MorphFrame& f) { return f.m_frameNo > frame; });
			if (inf != m_tracks[i].m_frames.end()) {
				auto& sup = inf--;
				if (inf != m_tracks[i].m_frames.end()) {
					// 補間
					float x = (frame - inf->m_frameNo) / (sup->m_frameNo - inf->m_frameNo);
					weight = glm::mix(inf->m_weight, sup->m_weight, x);
				}
				else {
					weight = sup->m_weight;
				}
			}
			else {
				if (m_tracks[i].m_frames.empty()) {
					// Do nothing
				}
				else {
					weight = m_tracks[i].m_frames[0].m_weight;
				}
			}

			if (weight > 0.0f) {
				for (const auto& elem : m_tracks[i].m_morph.m_elems) {
					int32_t idx = elem.m_idx * 3;
					transforms[idx + 0] += weight * elem.m_offset[0];
					transforms[idx + 1] += weight * elem.m_offset[1];
					transforms[idx + 2] += weight * elem.m_offset[2];
				}
			}
		}
	}

}
