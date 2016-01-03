//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdint.h>
#include "MotionData.h"
#include "ModelData/ModelData.h"

namespace MOES
{
	class BoneAnimator {
		MAKE_JAFOBJ(BoneAnimator);
	public:
		/// MotionData��Model���o�C���h����
		static Ptr create(const MotionData* motion, const ModelData* model);
		~BoneAnimator();

		/// @param frame [in] �t���[��
		/// @param transforms [out] �{�[���ό`�s��
		void getBoneTransforms(float frame, std::vector<glm::mat4> &transforms);

		int32_t maxFrameNo() const { return m_maxFrameNo; }

	private:
		//-----------------------------
		// private structures
		//-----------------------------
		// Bone�̐ڑ��֌W��\������Tree�̃m�[�h
		struct TreeNode {
			int32_t m_id;
			std::vector<TreeNode> m_children;
		};

		/// �{�[���p����vec3 �� quat �ŕ\������\���́B<br>
		/// �e�퉉�Z�q����`����
		struct BoneTransform {
			glm::vec3 m_pos;
			glm::quat m_quat;

			// Ctor.
			BoneTransform() : m_pos(), m_quat() {}
			BoneTransform(glm::vec3 pos, glm::quat quat) : m_pos(pos), m_quat(quat) {}

			// operators
			BoneTransform& operator *=(const BoneTransform& rhs)
			{
				this->m_pos += (this->m_quat * rhs.m_pos);
				this->m_quat *= rhs.m_quat;
				return *this;
			}
			BoneTransform operator *(const BoneTransform& rhs) const
			{
				return BoneTransform(this->m_pos + this->m_quat * rhs.m_pos, this->m_quat * rhs.m_quat);
			}
			glm::vec3 operator *(const glm::vec3 &rhs) const
			{
				return this->m_quat * (this->m_pos + rhs);
			}
			/// mat4 �̕ϊ��s��𐶐�����
			operator glm::mat4() const
			{
				glm::mat4 m = glm::mat4_cast(m_quat);
				m[3] = glm::vec4(m_pos, 1.0f);
				return m;
			}
		};
		// ����
		typedef BoneTransform BT;
		typedef std::vector<BT> BTList;

		/// �t���[���ł̃{�[���̎p����\���\����
		struct BoneFrame {
			int32_t m_frameNo;
			BT m_bt;
		};
		typedef std::vector<BoneFrame> BoneFrameList;

		/// �g���b�N�B�ЂƂ̃{�[���ɂ��ẴL�[�t���[���ł̎p���̃��X�g
		struct Track {
			BoneFrameList m_frames;
		};

		//-----------------------------
		// private functions
		//-----------------------------
		BoneAnimator(const MotionData* motion, const ModelData* model);

		/// �Ώۂ̃{�[���Ǝq���{�[����GlobalTransform��LocalTransfomr����Čv�Z����
		static void updateGlobalTransform(const Bone* bone, BT parentBT, const BTList &locals, BTList &globals);

		/// IK ���v�Z����
		void resolveIK(const Bone* bone, BTList &locals, BTList &globals);

		//-----------------------------
		// private variables
		//-----------------------------
		std::vector<Track> m_tracks;
		std::vector<Bone> m_bones;
		Bone m_rootBone;

		std::vector<glm::vec3> m_absPos;
		std::vector<glm::vec3> m_relPos;

		std::map<int32_t, IKBone> m_ikBones;

		int32_t m_maxFrameNo;
		int32_t m_maxPhase;

		//-----------------------------
		// BT helper functions
		//-----------------------------
		BT mix(const BT& a, const BT& b, float x)
		{
			return BT(glm::mix(a.m_pos, b.m_pos, x), glm::mix(a.m_quat, b.m_quat, x));
		}

		BT inverse(const BT& a)
		{
			return BT(-a.m_pos, glm::inverse(a.m_quat));
		}
	};
	typedef BoneAnimator::Ptr BoneAnimatorPtr;
}
