//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>
#include <vector>
#include <functional>

namespace MOES
{
	/// ボーン
	struct Bone {
		Bone()
			: m_idx(-1)
			, m_parentIdx(-1)
			, m_ikTargetIdx(-1)
			, m_phase(-1)
			, m_afterPhys(false)
			, m_parent(nullptr)
		{
		}

		void addChild(Bone& child)
		{
			child.m_parent = this;
			m_children.push_back(&child);
		}

		//-------------------
		/// 自身のボーンIndex
		int32_t m_idx;
		/// 親ボーンIndex。親がなければ -1
		int32_t m_parentIdx;
		/// IK Target のボーンIndex
		int32_t m_ikTargetIdx;

		/// 初期姿勢でのボーン位置
		float m_pos[3];

		/// どのフェーズでボーン処理を行うか
		int32_t m_phase;

		/// どのボーンから影響を受けるか
		int32_t m_effectorIdx;
		/// 影響を受ける度合い
		float m_effect;
		/// 位置が影響を受けるかどうか
		bool m_posEffect;
		/// 回転が影響を受けるかどうか
		bool m_rotEffect;
		/// ボーン処理を物理演算後に行うかどうか
		bool m_afterPhys;

		//-------------------
		// for Bone tree
		//-------------------
		Bone* m_parent;
		std::vector<Bone*> m_children;
	};

	/// ボーンツリーに対して再帰的に処理を行う
	/// @param bone [in] 処理対象のボーン
	/// @param preProc [in] 子ノードを処理する前に行う処理
	/// @param postProc [in] 子ノードを処理した後に行う処理
	inline void traverseBoneTree(Bone* bone, std::function<void(Bone*)> preProc, std::function<void(Bone*)> postProc)
	{
		if (!bone) { return; }
		preProc(bone);
		for (Bone* child : bone->m_children) {
			traverseBoneTree(child, preProc, postProc);
		}
		postProc(bone);
	}

	/// ボーンツリーを構築する<br>
	/// 各ボーンの親ID情報をもとにボーンインスタンスのツリー化を行う。<br>
	/// 親がいないボーンはrootBoneを親にする
	inline void constructBoneTree(Bone &rootBone, std::vector<Bone> &bones)
	{
		rootBone.m_parent = nullptr;
		rootBone.m_children.clear();
		for (auto &bone : bones) {
			bone.m_parent = nullptr;
			bone.m_children.clear();
		}
		for (auto &bone : bones) {
			// Get parent
			Bone* parent = nullptr;
			if (bone.m_parentIdx == -1) {
				parent = &rootBone;
			}
			else if (bone.m_parentIdx >= 0 && bone.m_parentIdx < (int)bones.size()) {
				parent = &bones[bone.m_parentIdx];
			}
			parent->addChild(bone);
		}

		rootBone.m_pos[0] = 0.0f;
		rootBone.m_pos[1] = 0.0f;
		rootBone.m_pos[2] = 0.0f;
	}

	struct IKLink {
		int32_t m_link; // LinkBoneID
		float m_lowerLimit[3];
		float m_upperLimit[3];
	};

	struct IKBone {
		int32_t m_id;
		int32_t m_target; // TargetBoneIdx
		int32_t m_loopCount;
		float m_step; // 単位角 (rad)
		std::vector<IKLink> m_link;
	};
}
