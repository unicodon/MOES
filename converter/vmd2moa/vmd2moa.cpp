//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include <SDKDDKVer.h>
#include <windows.h>

#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <tchar.h>

#include <algorithm>
#include <vector>
#include <map>

#include <MOAData.h>
#include "../common/util.h"

using namespace Util;

namespace VMD
{
	std::string toUTF8String(const std::string& str)
	{
		int wclen = MultiByteToWideChar(932, 0, str.c_str(), str.length(), 0, 0);
		wchar_t *wcbuf = new wchar_t[wclen];
		wclen = MultiByteToWideChar(932, 0, str.c_str(), str.length(), wcbuf, wclen);
		int len = WideCharToMultiByte(CP_UTF8, 0, wcbuf, wclen, 0, 0, NULL, NULL);
		std::string out;
		out.resize(len);
		WideCharToMultiByte(CP_UTF8, 0, wcbuf, wclen, &out[0], len, NULL, NULL);
		delete[] wcbuf;
		return out;
	}

	struct Header {
		Header(Reader& reader)
		{
			char magic[30];
			char name[20];
			INFO(_T("============HEADER============\n"));
			// Check magic
			reader.read(magic, 30);
			const char* kMagic = "Vocaloid Motion Data 0002";
			if (strcmp(magic, kMagic) != 0) {
				FATAL(_T("Unknown magic%s\n"));
			}

			// Read name
			reader.read(name, 20);
			m_name = toUTF8String(name);
			printf("VMD Name:%s\n", m_name.c_str());
		}
		std::string m_name;
	};

	struct BoneFrame {
		std::string m_name;
		uint32_t m_frame;
		float m_pos[3];
		float m_quat[4];
		char m_data[64];
	};

	struct IKFrame {
		std::string m_name;
		uint32_t m_frame;
		bool m_onoff;
	};

	struct MorphFrame {
		std::string m_name;
		uint32_t m_frame;
		float m_weight;
	};

	struct BoneMotion {
		BoneMotion(Reader& reader)
		{
			uint32_t count;
			// dictionary —p
			uint32_t id = 0;

			//------------------------
			// BoneFrame
			//------------------------
			if (!reader.tryRead<uint32_t>(&count, 1)) { return; }
			m_frames.resize(count);
			for (auto &frame : m_frames) {
				char name[16];
				name[15] = '\0';
				reader.read(name, 15);
				frame.m_name = toUTF8String(name);
				frame.m_frame = reader.read<uint32_t>();
				reader.read(frame.m_pos, 3);
				reader.read(frame.m_quat + 1, 3);
				reader.read(frame.m_quat, 1);
				reader.read(frame.m_data, 64);

				if (m_dict.find(frame.m_name) == m_dict.end()) {
					m_dict[frame.m_name] = id++;
				}
			}
			printf("Bone frames loaded.\n");

#if 0
			//------------------------
			// ShowIKFrame
			//------------------------
			if (!reader.tryRead<uint32_t>(&count, 1)) { return; }
			for (uint32_t i = 0; i < count; i++) {
				int32_t frame = reader.read<int32_t>();
				reader.skip(1); // show on/off
				uint32_t ikCount = reader.read<uint32_t>();
				for (uint32_t j = 0; j < ikCount; j++) {
					m_ikFrames.push_back(IKFrame());
					auto& ikFrame = m_ikFrames.back();
					ikFrame.m_frame = frame;

					char name[20];
					reader.read(name, 20);
					ikFrame.m_name = toUTF8String(name);
					ikFrame.m_onoff = (bool)(reader.read<uint8_t>() != 0);


					// Making the name dictionary.
					if (m_boneDict.find(ikFrame.m_name) == m_boneDict.end()) {
						m_boneDict[ikFrame.m_name] = boneId++;
					}
				}
			}
			reader.skip(count * 9);
#endif
		}
		std::vector<BoneFrame> m_frames;
		std::map<std::string, uint32_t> m_dict;
	};

	struct MorphMotion {
		MorphMotion(Reader& reader)
		{
			uint32_t count;
			uint32_t id = 0;

			//------------------------
			// MorphFrame
			//------------------------
			if (!reader.tryRead<uint32_t>(&count, 1)) { return; }
			m_frames.resize(count);
			for (auto &frame : m_frames) {
				char name[16];
				name[15] = '\0';
				reader.read(name, 15);
				frame.m_name = toUTF8String(name);
				frame.m_frame = reader.read<uint32_t>();
				frame.m_weight = reader.read<float>();

				if (m_dict.find(frame.m_name) == m_dict.end()) {
					m_dict[frame.m_name] = id++;
				}
			}

			printf("Morph frames loaded.\n");
		}
		std::vector<MorphFrame> m_frames;
		std::map<std::string, uint32_t> m_dict;
	};

	struct VMDInfo {
		VMDInfo(Reader& reader)
			: m_header(reader)
			, m_bone(reader)
			, m_morph(reader)
		{ }
		Header m_header;
		BoneMotion m_bone;
		MorphMotion m_morph;
	};
}

//-------------------------------------
//-------------------------------------
namespace MOA
{
	void vmd2moa(const VMD::VMDInfo& vmd, FILE* fp)
	{
		// Writing FileHeader
		FileHeader fh = { kMagic, kVersion };
		fwrite(&fh, sizeof fh, 1, fp);

		// Writing InfoHeader
		InfoHeader ih;
		ih.m_boneFrameCount = vmd.m_bone.m_frames.size();
		ih.m_boneNameCount = vmd.m_bone.m_dict.size();
		ih.m_morphFrameCount = vmd.m_morph.m_frames.size();
		ih.m_morphNameCount = vmd.m_morph.m_dict.size();
		fwrite(&ih, sizeof ih, 1, fp);

		// Writing Bone info
		for (const auto& frame : vmd.m_bone.m_frames) {
			auto it = vmd.m_bone.m_dict.find(frame.m_name);
			if (it == vmd.m_bone.m_dict.end()) {
				throw;
			}
			BoneFrame f;
			f.m_frameNo = frame.m_frame;
			f.m_boneId = it->second;
			memcpy(f.m_pos, frame.m_pos, sizeof f.m_pos);
			memcpy(f.m_quat, frame.m_quat, sizeof f.m_quat);
			fwrite(&f, sizeof f, 1, fp);
		}
		for (auto it : vmd.m_bone.m_dict) {
			Name name;
			name.m_id = it.second;
			name.m_reserved = 0;
			if (it.first.size() >= sizeof name.m_name) {
				throw;
			}
			strncpy(name.m_name, it.first.c_str(), sizeof name.m_name);
			fwrite(&name, sizeof name, 1, fp);
		}

		// Writing Morph info
		for (const auto& frame : vmd.m_morph.m_frames) {
			auto it = vmd.m_morph.m_dict.find(frame.m_name);
			if (it == vmd.m_morph.m_dict.end()) {
				throw;
			}
			MorphFrame f;
			f.m_frameNo = frame.m_frame;
			f.m_morphId = it->second;
			f.m_weight = frame.m_weight;
			fwrite(&f, sizeof f, 1, fp);
		}
		for (auto it : vmd.m_morph.m_dict) {
			Name name;
			name.m_id = it.second;
			name.m_reserved = 0;
			if (it.first.size() >= sizeof name.m_name) {
				throw;
			}
			strncpy(name.m_name, it.first.c_str(), sizeof name.m_name);
			fwrite(&name, sizeof name, 1, fp);
		}
	}
}


int convert(int argc, _TCHAR* argv[])
{
	Reader reader(argv[1]);
	VMD::VMDInfo vmdInfo(reader);

	tstring baseDir = getBaseDir(argv[1]);
	SetCurrentDirectory(baseDir.c_str());

	tstring outFile = getFname(argv[1]);
	outFile += _T(".moa");

	FILE* fout;
	if (_tfopen_s(&fout, outFile.c_str(), _T("wb"))) {
		FATAL(_T("Cannot open %s\n"), argv[2]);
	}
	MOA::vmd2moa(vmdInfo, fout);
	fclose(fout);

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1) {
		INFO(_T("vmd2moa version 0x%08X"), MOA::kVersion);
		Sleep(3 * 1000);
		return -1;
	}

	// Set console CP to UTF-8
	UINT cp = CP_UTF8;
	::SetConsoleCP(cp);
	::SetConsoleOutputCP(cp);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD nFont = 12;
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = nFont;
	cfi.dwFontSize = ::GetConsoleFontSize(hConsole, nFont);
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = 400;
	lstrcpy(cfi.FaceName, TEXT("MS Gothic"));
	::SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

	return convert(argc, argv);
}

