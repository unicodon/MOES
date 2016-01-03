//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>

namespace MOA
{
	const uint32_t kVersion = 0x00000100;
	const uint32_t kMagic = 'MOA ';

	//---------------------------------
	// ファイルヘッダ
	//---------------------------------
	struct FileHeader {
		uint32_t m_magic;
		uint32_t m_version;
	};

	//---------------------------------
	// ファイルヘッダ
	//---------------------------------
	struct InfoHeader {
		uint32_t m_boneFrameCount;
		uint32_t m_boneNameCount;
		uint32_t m_morphFrameCount;
		uint32_t m_morphNameCount;
	};

	//---------------------------------
	// IDとName の関連付け
	//---------------------------------
	struct Name {
		uint32_t m_id;
		uint32_t m_reserved;
		char m_name[32];
	};

	//---------------------------------
	// BoneFrame
	//---------------------------------
	struct BoneFrame {
		int32_t m_frameNo;
		uint32_t m_boneId;
		float m_pos[3];
		float m_quat[4];
	};

	//---------------------------------
	// MorphFrame
	//---------------------------------
	struct MorphFrame {
		int32_t m_frameNo;
		uint32_t m_morphId;
		float m_weight;
	};

	//---------------------------------
	// IKFrame
	//---------------------------------
	struct IKFrame {
		int32_t m_frameNo;
		uint32_t m_boneId;
		int32_t m_onoff;
	};
}
