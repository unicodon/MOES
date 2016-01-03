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
	/// �{�[��
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
		/// ���g�̃{�[��Index
		int32_t m_idx;
		/// �e�{�[��Index�B�e���Ȃ���� -1
		int32_t m_parentIdx;
		/// IK Target �̃{�[��Index
		int32_t m_ikTargetIdx;

		/// �����p���ł̃{�[���ʒu
		float m_pos[3];

		/// �ǂ̃t�F�[�Y�Ń{�[���������s����
		int32_t m_phase;

		/// �ǂ̃{�[������e�����󂯂邩
		int32_t m_effectorIdx;
		/// �e�����󂯂�x����
		float m_effect;
		/// �ʒu���e�����󂯂邩�ǂ���
		bool m_posEffect;
		/// ��]���e�����󂯂邩�ǂ���
		bool m_rotEffect;
		/// �{�[�������𕨗����Z��ɍs�����ǂ���
		bool m_afterPhys;

		//-------------------
		// for Bone tree
		//-------------------
		Bone* m_parent;
		std::vector<Bone*> m_children;
	};

	/// �{�[���c���[�ɑ΂��čċA�I�ɏ������s��
	/// @param bone [in] �����Ώۂ̃{�[��
	/// @param preProc [in] �q�m�[�h����������O�ɍs������
	/// @param postProc [in] �q�m�[�h������������ɍs������
	inline void traverseBoneTree(Bone* bone, std::function<void(Bone*)> preProc, std::function<void(Bone*)> postProc)
	{
		if (!bone) { return; }
		preProc(bone);
		for (Bone* child : bone->m_children) {
			traverseBoneTree(child, preProc, postProc);
		}
		postProc(bone);
	}

	/// �{�[���c���[���\�z����<br>
	/// �e�{�[���̐eID�������ƂɃ{�[���C���X�^���X�̃c���[�����s���B<br>
	/// �e�����Ȃ��{�[����rootBone��e�ɂ���
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
		float m_step; // �P�ʊp (rad)
		std::vector<IKLink> m_link;
	};
}
