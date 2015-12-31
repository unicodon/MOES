//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "BoneAnimator.h"

#include <algorithm>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <JTL.h>

#include "debug.h"

namespace MOES
{
	BoneAnimator::~BoneAnimator()
	{
	}

	BoneAnimator::BoneAnimator(const MotionData* motion, const ModelData* model)
	{
		m_maxFrameNo = 0;
		m_maxPhase = 0;

		// BoneをModelからコピー
		m_bones = model->bones();
		size_t boneCount = m_bones.size();
		// BoneTree構築
		constructBoneTree(m_rootBone, m_bones);

		// 初期ボーン姿勢を構築
		m_absPos.resize(boneCount);
		m_relPos.resize(boneCount);
		traverseBoneTree(&m_rootBone, 
			[&](Bone* bone) {
			int32_t id = bone->m_idx;
			if (id < 0) {
				return;
			}
			m_absPos[id] = glm::vec3(bone->m_pos[0], bone->m_pos[1], bone->m_pos[2]);
			m_relPos[id] = m_absPos[id];
			bone = bone->m_parent;
			if (bone) {
				m_relPos[id] += -glm::vec3(bone->m_pos[0], bone->m_pos[1], bone->m_pos[2]);
			}
			},
			[](Bone*) {});

		// MotionのBoneIDとModelのBoneIDをBone名でバインドする
		auto boneNames = motion->boneNames();
		std::vector<int32_t> modelBoneIds;
		modelBoneIds.resize(boneNames.size());
		for (auto &boneName : boneNames) {
			const char* name = boneName.m_name;
			uint32_t id = boneName.m_id;
			int32_t resolvedId = model->boneNames().lookUp(name);
			modelBoneIds[id] = resolvedId;
			if (resolvedId < 0) {
				DBG_INFO("Missing bone %s\n", name);
			}
		}

		// Bone設定
		m_tracks.resize(boneCount);
		for (int32_t i = 0; i < (int)boneCount; i++) {
			int32_t ikBoneId = model->getIkBoneId(i);
			if (ikBoneId >= 0) {
				m_ikBones[i] = model->ikBones()[ikBoneId];
			}

			m_maxPhase = std::max<int32_t>(m_maxPhase, m_bones[i].m_phase);
		}

		// BoneFrameを取り込む
		auto boneFrames = motion->boneFrames();
		for (auto &boneFrame : boneFrames) {
			int32_t id = modelBoneIds[boneFrame.m_boneId];
			m_maxFrameNo = std::max<int32_t>(m_maxFrameNo, boneFrame.m_frameNo);
			if (id < 0) {
				continue;
			}

			auto &frames = m_tracks[id].m_frames;
			frames.push_back(BoneFrame());
			auto &frame = frames.back();
			frame.m_frameNo = boneFrame.m_frameNo;
			frame.m_bt.m_pos = m_relPos[id] + glm::vec3(boneFrame.m_pos[0], boneFrame.m_pos[1], boneFrame.m_pos[2]);
			frame.m_bt.m_quat = glm::quat(boneFrame.m_quat[0], boneFrame.m_quat[1], boneFrame.m_quat[2], boneFrame.m_quat[3]);
		}

		for (auto& elem : m_tracks) {
			std::sort(elem.m_frames.begin(), elem.m_frames.end(),
				[](const BoneFrame& l, const BoneFrame& r) { return l.m_frameNo < r.m_frameNo; }
			);
		}

	}

	BoneAnimator::Ptr BoneAnimator::create(const MotionData* motion, const ModelData* model)
	{
		BoneAnimator* ptr = new BoneAnimator(motion, model);
		return Ptr(ptr);
	}

	void BoneAnimator::updateGlobalTransform(const Bone* bone, BT parentBT, const BTList &locals, BTList &globals)
	{
		int32_t i = bone->m_idx;
		if (i >= 0) {
			parentBT = parentBT * locals[i];
			globals[i] = parentBT;
		}
		for (const auto &child : bone->m_children) {
			updateGlobalTransform(child, parentBT, locals, globals);
		}
	}

	void BoneAnimator::resolveIK(const Bone* bone, BTList &locals, BTList &globals)
	{
		assert(bone);
		int32_t boneId = bone->m_idx;
		const IKBone& ik = m_ikBones[boneId];
		float step = ik.m_step;
		int32_t loopCount = ik.m_loopCount;
		int32_t targetBoneId = ik.m_target; // ターゲットボーンID
		glm::vec3 globalIKPos = globals[boneId].m_pos; // IKボーン座標

		// ターゲットボーンの親はリンク0という前提
		if (ik.m_link.empty() || m_bones[targetBoneId].m_parentIdx != ik.m_link[0].m_link) {
			return;
		}

		size_t linkCount = ik.m_link.size();
		for (int i = 0; i < loopCount; i++) {
			for (int l = 0; l < (int)linkCount; l++) {
				auto &link = ik.m_link[l];

				// LinkBone座標への変換行列
				BT inv = inverse(globals[link.m_link]);
				// LinkBone座標系でのIKボーン位置
				glm::vec3 ikPos = inv * globalIKPos;
				// LinkBone座標系でのIKボーン位置
				glm::vec3 targetPos = inv * globals[targetBoneId].m_pos;

				// ターゲットボーン方向をIKボーン方向に回転させるQuatを求める
				glm::vec3 from = glm::normalize(targetPos);
				glm::vec3 to = glm::normalize(ikPos);
				glm::quat q = glm::quat(from, to);

				// Step制限に収める
				glm::vec3 euler = glm::eulerAngles(q);
				euler = glm::clamp(euler, glm::vec3(-step), glm::vec3(step));
				q = glm::quat(euler);
				locals[link.m_link].m_quat *= q;

				// 角度制限に収める
				euler = glm::eulerAngles(locals[link.m_link].m_quat);
				euler = glm::clamp(euler,
					glm::vec3(link.m_lowerLimit[0], link.m_lowerLimit[1], link.m_lowerLimit[2]),
					glm::vec3(link.m_upperLimit[0], link.m_upperLimit[1], link.m_upperLimit[2]));
				locals[link.m_link].m_quat = glm::quat(euler);

				// Globalをリンク先まで更新
				BT globalBT = globals[m_bones[link.m_link].m_parentIdx];
				for (int k = l; k >= 0; k--) {
					int32_t id = ik.m_link[k].m_link;
					globalBT = globalBT * locals[id];
					globals[id] = globalBT;
				}
				// Target を更新
				globals[targetBoneId] = globalBT * locals[targetBoneId];
			}
		}
	}


	void BoneAnimator::getBoneTransforms(float frame, std::vector<glm::mat4> &transforms)
	{
		size_t boneCount = m_tracks.size();
		BTList locals;
		BTList globals;
		locals.resize(boneCount);
		globals.resize(boneCount);

		// 初期値設定
		for (size_t i = 0; i < boneCount; i++) {
			// 初期値を設定
			BT bt;
			bt.m_pos = m_relPos[i];

			auto &inf = std::find_if(m_tracks[i].m_frames.begin(), m_tracks[i].m_frames.end(), [frame](const BoneFrame& f) { return f.m_frameNo > frame; });
			if (inf != m_tracks[i].m_frames.end()) {
				auto& sup = inf--;
				if (inf != m_tracks[i].m_frames.end()) {
					// 補間
					float x = (frame - inf->m_frameNo) / (sup->m_frameNo - inf->m_frameNo);
					bt = mix(inf->m_bt, sup->m_bt, x);
				}
				else {
					bt = sup->m_bt;
				}
			}
			else {
				if (m_tracks[i].m_frames.empty()) {
					// Do nothing
				}
				else {
					bt = m_tracks[i].m_frames[0].m_bt;
				}
			}
			locals[i] = bt;
		}
		// ローカル行列を合成する
		updateGlobalTransform(&m_rootBone, BT(), locals, globals);

		// Bone 制御
		for (int phys = 0; phys < 2; phys++) {
			for (int32_t phase = 0; phase < m_maxPhase; phase++) {
				for (size_t i = 0; i < boneCount; i++) {
					Bone& bone = m_bones[i];
					if (bone.m_phase != phase || (int)bone.m_afterPhys != phys) {
						continue;
					}

					// 別ボーンの影響を反映
					if (bone.m_rotEffect) {
						locals[i].m_quat = glm::slerp(
							locals[i].m_quat,
							locals[i].m_quat * locals[bone.m_effectorIdx].m_quat,
							bone.m_effect);
						updateGlobalTransform(&bone, globals[bone.m_parentIdx], locals, globals);
					}
					else if (bone.m_posEffect) {
						locals[i].m_pos += locals[bone.m_effectorIdx].m_pos * bone.m_effect;
						updateGlobalTransform(&bone, globals[bone.m_parentIdx], locals, globals);
					}
					
					// IK
					if (bone.m_ikTargetIdx >= 0) {
						resolveIK(&bone, locals, globals);
					}
				}
			}
		}

		// 最終出力
		transforms.resize(boneCount);
		for (size_t i = 0; i < boneCount; i++) {
			transforms[i] = globals[i];
			transforms[i] = glm::translate(transforms[i], -m_absPos[i]);
		}
	}
}
