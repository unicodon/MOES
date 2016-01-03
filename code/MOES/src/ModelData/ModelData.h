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
	/// モデルデータを格納するクラス
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

		/// IK BoneがBoneにバインドされていればそのIdを、そうでなければ-1を返す
		int32_t getIkBoneId(int32_t i) const
		{
			if (i >= 0 && i < (int)m_bones.size()) {
				return m_bones[i].m_ikTargetIdx;
			}
			return -1;
		}

	private:
		/// 頂点数
		int32_t m_vertexCount;
		/// 面数
		int32_t m_faceCount;

		/// メッシュ
		std::vector<Mesh> m_meshes;
		/// テクスチャ
		std::vector<MOEGUL::Texture> m_textures;
		/// ボーン
		std::vector<Bone> m_bones;
		Dictionary m_boneNames;
		/// ルートボーン
		Bone m_rootBone;
		/// IK ボーン 
		std::vector<IKBone> m_ikBones;

		/// VertexMorph
		std::vector<VertexMorph> m_vertexMorphs;
		Dictionary m_vertexMorphNames;
	};
	typedef ModelData::Ptr ModelDataPtr;
}
