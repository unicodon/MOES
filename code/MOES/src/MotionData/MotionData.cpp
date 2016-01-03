//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "MotionData.h"

#include <JAFFile.h>
#include <JTL.h>

namespace MOES
{
	MotionData::MotionData()
	{
	}

	MotionData::~MotionData()
	{
	}

	MotionData::Ptr MotionData::create(FILE* fp)
	{
		Ptr ptr(new MotionData);
		if (!ptr) {
			return Ptr();
		}

		MOA::FileHeader fh;
		if (JAF::fread(&fh, sizeof fh, 1, fp) != 1) {
			return Ptr();
		}
		if (fh.m_magic != MOA::kMagic) {
			return Ptr();
		}
		if (fh.m_version != MOA::kVersion) {
			return Ptr();
		}

		MOA::InfoHeader ih;
		if (JAF::fread(&ih, sizeof ih, 1, fp) != 1) {
			return Ptr();
		}

		ptr->m_boneFrames.resize(ih.m_boneFrameCount);
		if (JAF::fread(&ptr->m_boneFrames[0], sizeof(MOA::BoneFrame), ih.m_boneFrameCount, fp) != ih.m_boneFrameCount) {
			return Ptr();
		}

		ptr->m_boneNames.resize(ih.m_boneNameCount);
		if (JAF::fread(&ptr->m_boneNames[0], sizeof(MOA::Name), ih.m_boneNameCount, fp) != ih.m_boneNameCount) {
			return Ptr();
		}

		ptr->m_morphFrames.resize(ih.m_morphFrameCount);
		if (JAF::fread(&ptr->m_morphFrames[0], sizeof(MOA::MorphFrame), ih.m_morphFrameCount, fp) != ih.m_morphFrameCount) {
			return Ptr();
		}

		ptr->m_morphNames.resize(ih.m_morphNameCount);
		if (JAF::fread(&ptr->m_morphNames[0], sizeof(MOA::Name), ih.m_morphNameCount, fp) != ih.m_morphNameCount) {
			return Ptr();
		}

		return ptr;
	}
}
