//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdlib.h>
#include <map>
#include <string>
#include <vector>
#include <JAFObj.h>
#include <MOEGUL/GL.h>
#include <MOMData.h>
#include "Dictionary.h"
#include "Mesh.h"
#include "Bone.h"
#include "Morph.h"

namespace MOES
{
	/// ���f���f�[�^���i�[����N���X
	class ModelData {
		MAKE_JAFOBJ(ModelData);
	public:
		static Ptr create(FILE* fp);
		~ModelData();

		//---------------------------------
		// Accessors
		//---------------------------------
		const Bone* rootBone() const{ return &m_rootBone; }
		const std::vector<Mesh> & meshes() const { return m_meshes; }
		const std::vector<MOEGUL::Texture> textures() const { return m_textures; }
		const std::vector<Bone> & bones() const { return m_bones; }
		const std::vector<IKBone> & ikBones() const { return m_ikBones; }
		const std::vector<VertexMorph> & vertexMorphs() const { return m_vertexMorphs; }
		const Dictionary& boneNames() const { return m_boneNames; }
		const Dictionary& vertexMorphNames() const { return m_vertexMorphNames; }

		int32_t vertexCount() const { return m_vertexCount; }
		int32_t faceCount() const { return m_faceCount; }

		/// IK Bone��Bone�Ƀo�C���h����Ă���΂���Id���A�����łȂ����-1��Ԃ�
		int32_t getIkBoneId(int32_t i) const
		{
			if (i >= 0 && i < (int)m_bones.size()) {
				return m_bones[i].m_ikTargetIdx;
			}
			return -1;
		}

	private:
		/// ���_��
		int32_t m_vertexCount;
		/// �ʐ�
		int32_t m_faceCount;

		/// ���b�V��
		std::vector<Mesh> m_meshes;
		/// �e�N�X�`��
		std::vector<MOEGUL::Texture> m_textures;
		/// �{�[��
		std::vector<Bone> m_bones;
		Dictionary m_boneNames;
		/// ���[�g�{�[��
		Bone m_rootBone;
		/// IK �{�[�� 
		std::vector<IKBone> m_ikBones;

		/// VertexMorph
		std::vector<VertexMorph> m_vertexMorphs;
		Dictionary m_vertexMorphNames;
	};
	typedef ModelData::Ptr ModelDataPtr;
}
