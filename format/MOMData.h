//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>

namespace MOM
{
	const uint32_t kVersion = 0x00000100;
	const uint32_t kMagic = 'MOM ';

	//---------------------------------
	// ファイルヘッダ
	//---------------------------------
	struct FileHeader {
		uint32_t m_magic;
		uint32_t m_version;
	};

	//---------------------------------
	// チャンク定義
	//---------------------------------
	struct ChunkHeader {
		uint32_t m_type;
		uint32_t m_index; // 同一チャンクタイプ内での識別番号
		uint32_t m_contentSize; // 4 byte-aligned
	};

	//---------------------------------
	// チャンクの出現順
	// 0: (FileHeader)
	// 1: InfoHeader
	// 2: Vertex
	// 3: Face
	// 4: Material
	// 5: Bone
	//  : 以下任意の順番 (ただし同一チャンク内のindex は連番で出現すること)
	//---------------------------------


	//---------------------------------
	// InfoHeaderチャンク:
	//   InfoHeader * 1
	//---------------------------------
	const uint32_t CHUNK_TYPE_INFO = 'INFO';
	struct InfoHeader {
		int32_t m_vertexCount;
		int32_t m_faceCount;
		int32_t m_textureCount;
		int32_t m_materialCount;
		int32_t m_boneCount;
	};

	//---------------------------------
	// Vertexチャンク:
	//   Vertex * m_vertexCount
	//---------------------------------
	const uint32_t CHUNK_TYPE_VERT = 'VERT';
	struct Vertex {
		float m_pos[3];
		float m_normal[3];
		float m_uv[2];
		int32_t m_boneIndex[4];
		float m_boneWeight[4];
	};

	//---------------------------------
	// Faceチャンク:
	//   Face * m_faceCount
	//---------------------------------
	const uint32_t CHUNK_TYPE_FACE = 'FACE';
	struct Face {
		int32_t m_indices[3];
	};

	//---------------------------------
	// Materialチャンク:
	//   Material * m_materialCount
	//---------------------------------
	const uint32_t CHUNK_TYPE_MATE = 'MATE';
	struct Material {
		enum DrawFlag {
			kDrawBackFace = 0x01
		};

		float m_diffuse[4];
		float m_specular[4];
		float m_ambient[4];
		uint8_t m_drawFlags;

		float m_edgeColor[4];
		float m_edgeSize;

		int32_t m_textureIndex;
		int32_t m_sphereTextureIndex;

		uint8_t m_sphereMode;
		uint8_t m_sharedToonFlag;

		int32_t m_faceIndex;
		int32_t m_faceCount;
	};

	//---------------------------------
	// Boneチャンク:
	//   Bone * m_boneCount
	//---------------------------------
	const uint32_t CHUNK_TYPE_BONE = 'BONE';
	struct Bone {
		enum BoneFlag {
			kBoneFlagIK = 0x0020,
			kBoneFlagRotEffect = 0x0100,
			kBoneFlagPosEffect = 0x0200,
			kBoneFlagAfterPhys = 0x1000,
		};

		/// 識別名
		/// @note NUL-terminated UTF-8
		char m_name[32];

		float m_pos[3];
		int32_t m_parent; // Parent bone ID
		uint32_t m_phase;
		uint32_t m_flags;
		int32_t m_effector;
		float m_effect;
		int32_t m_ik;
	};

	//---------------------------------
	// VertexMorphチャンク:
	//---------------------------------
	const uint32_t CHUNK_TYPE_VMOR = 'VMOR';
	struct VertexMorphHeader {
		/// 識別名
		/// @note NUL-terminated UTF-8
		char m_name[32];

		/// 後続のVertexMorphElem 数
		uint32_t m_count;
	};
	struct VertexMorphElem {
		int32_t m_index;
		float m_offset[3];
	};

	//---------------------------------
	// IKDataチャンク
	//---------------------------------
	const uint32_t CHUNK_TYPE_IKDT = 'IKDT';
	struct IKData {
		uint32_t m_id;
		int32_t m_target; // TargetBoneID
		int32_t m_loopCount;
		float m_step; // 単位角 (rad)
		uint32_t m_count; // 後続のIKLink数
	};

	struct IKLink {
		int32_t m_link; // LinkBoneID
		float m_lowerLimit[3];
		float m_upperLimit[3];
	};

	//---------------------------------
	// RawDataチャンク
	//---------------------------------
	// テクスチャ(DDS)
	const uint32_t CHUNK_TYPE_XDDS = 'XDDS';
	// コメント
	const uint32_t CHUNK_TYPE_COMM = 'COMM';
} // MOM
