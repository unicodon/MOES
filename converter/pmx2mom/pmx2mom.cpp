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

#include <MOMData.h>
#include "../common/Util.h"

using namespace Util;

static Util::tstring s_texconvPath;

namespace pmx
{
	struct Header;

	enum CharEncoding {
		kUTF16LE,
		kUTF8
	};

	std::string readPmxString(Reader& reader, CharEncoding encoding)
	{
		std::string str;

		uint32_t len = reader.read<uint32_t>();
		if (len == 0) {
			return str;
		}
		if ((encoding == kUTF16LE) && (len % 2)) {
			FATAL(_T("Corrupted data\n"));
		}
		if (len > 100000) {
			FATAL(_T("Corrupted data\n"));
		}

		char* data = new char[len];
		reader.read(data, len);
		if (encoding == kUTF16LE) {
			char* utf8Data = new char[len * 2];
			char* out = utf8Data;
			uint16_t* in = (uint16_t*)data;
			len /= 2;
			for (uint32_t i = 0; i < len; i++) {
				uint16_t c = *in++;
				if (c <= 0x7F) {
					out[0] = (0x7F & c);
					out += 1;
				}
				else if (c <= 0x7FF) {
					out[1] = (c & 0x3F) | 0x80; c >>= 6;
					out[0] = c | 0xC0;
					out += 2;
				}
				else if (c <= 0xFFFF) {
					out[2] = (c & 0x3F) | 0x80; c >>= 6;
					out[1] = (c & 0x3F) | 0x80; c >>= 6;
					out[0] = c | 0xE0;
					out += 3;
				}
			}
			uint32_t utf8Len = out - utf8Data;
			str.assign(utf8Data, utf8Len);
			delete[] utf8Data;
		}
		else if (encoding == kUTF8) {
			str.assign(data, len);
		}
		else {
			FATAL(_T("Corrupted data\n"));
		}
		delete[] data;

		if (str.size()) {
			int wcharcount = str.size();
			wchar_t *tempWstr = new wchar_t[wcharcount];
			wcharcount = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), tempWstr, wcharcount);
			int charcount = wcharcount * 4;
			char *tempStr = new char[charcount + 1];
			memset(tempStr, 0, charcount + 1);
			WideCharToMultiByte(CP_OEMCP, 0, tempWstr, wcharcount, tempStr, charcount, 0, 0);
			printf("%s\n", tempStr);
			delete[] tempStr;
			delete[] tempWstr;
		}

		return str;
	}
	
	//-----------------------------------------------------
	// PMX構造体宣言
	//-----------------------------------------------------

	struct Vertex : public MOM::Vertex {
		typedef MOM::Vertex BaseType;
		Vertex(Reader& reader, Header& header);
	};

	struct Face : public MOM::Face {
		typedef MOM::Face BaseType;
		Face(Reader& reader, Header& header);
	};

	struct Material : public MOM::Material {
		typedef MOM::Material BaseType;
		Material(Reader& reader, Header& header);
	};

	struct IKLink : public MOM::IKLink {
		typedef MOM::IKLink BaseType;
		IKLink(Reader& reader, Header& header);
	};

	struct IKData : public MOM::IKData {
		typedef MOM::IKData BaseType;
		IKData(Reader& reader, Header& header);
		std::vector<IKLink> m_link;
	};

	struct Bone : MOM::Bone {
		typedef MOM::Bone BaseType;
		Bone(Reader& reader, Header& header);
	};

	//-----------------------------------------------------
	// Header
	//-----------------------------------------------------
	struct Header {
		Header(Reader& reader)
		{
			INFO(_T("============HEADER============\n"));
			// Check magic
			const char PMX_[4] = { 'P', 'M', 'X', ' ' };
			char magic[4];
			reader.read(magic, 4);
			if (memcmp(magic, PMX_, 4)) {
				FATAL(_T("Unknown magic%s\n"));
			}

			// Check version
			float version = reader.read<float>();
			m_version[0] = (uint8_t)version;
			m_version[1] = (uint8_t)((version - m_version[0]) * 10);

			if (m_version[0] != 2) {
				FATAL(_T("Unsupported version %f\n"), version);
			}
			if (m_version[1] != 0) {
				FATAL(_T("Unsupported version %f\n"), version);
			}
			INFO(_T("Version = %f\n"), version);

			//
			uint8_t len = reader.read<uint8_t>();
			if (len != 8) {
				FATAL(_T("Corrupted data\n"));
			}

			// CharEncode
			uint8_t data;
			data = reader.read<uint8_t>();
			switch (data) {
			case 0:
				m_charEncoding = kUTF16LE;
				INFO(_T("Encode = UTF16LE\n"), version);
				break;
			case 1:
				m_charEncoding = kUTF8;
				INFO(_T("Encode = UTF8\n"), version);
				break;
			default:
				FATAL(_T("Corrupted data\n"));
				break;
			}

			// Extra UV
			data = reader.read<uint8_t>();
			INFO(_T("Extra UV = %d\n"), data);
			if (data > 0) {
				FATAL(_T("Unsupported data\n"));
			}
			m_extraUV = data;

			// Index size
			for (int i = 0; i < 6; i++) {
				data = reader.read<uint8_t>();
				if (data != 1 && data != 2 && data != 4) {
					FATAL(_T("Corrupted data\n"));
				}
				INFO(_T("Index size[%d] = %d\n"), i, data);
				m_indexSizes[i] = data;
			}

			m_vertexCount = 0;
			m_faceCount = 0;
			m_textureCount = 0;
			m_materialCount = 0;
			m_materialFaceCount = 0;
			m_boneCount = 0;
			m_morphCount = 0;
		}

		uint8_t vertexIndexSize() const { return m_indexSizes[0]; }
		uint8_t textureIndexSize() const { return m_indexSizes[1]; }
		uint8_t materialIndexSize() const { return m_indexSizes[2]; }
		uint8_t boneIndexSize() const { return m_indexSizes[3]; }
		uint8_t morphIndexSize() const { return m_indexSizes[4]; }

		uint8_t m_version[2]; // 0: Major, 1: Minor
		CharEncoding m_charEncoding;
		uint8_t m_extraUV;
		uint8_t m_indexSizes[6];

		int32_t m_vertexCount;
		int32_t m_faceCount;
		int32_t m_textureCount;
		int32_t m_materialCount;
		int32_t m_materialFaceCount;
		int32_t m_boneCount;
		int32_t m_morphCount;

		std::vector<IKData> m_ikData;
	};

	int32_t readVertexIndex(Reader& reader, Header& header)
	{
		uint8_t indexSize = header.vertexIndexSize();

		switch (indexSize) {
		case 1:
			return reader.read<uint8_t>();
		case 2:
			return reader.read<uint16_t>();
		case 4:
			return reader.read<int32_t>();
		}
		FATAL(_T("UNEXPECTED INDEX SIZE"));
	}

	int32_t readBoneIndex(Reader& reader, Header& header)
	{
		uint8_t indexSize = header.boneIndexSize();

		switch (indexSize) {
		case 1:
			return reader.read<int8_t>();
		case 2:
			return reader.read<int16_t>();
		case 4:
			return reader.read<int32_t>();
		}
		FATAL(_T("UNEXPECTED INDEX SIZE"));
	}

	//-----------------------------------------------------
	// PMX構造体コンストラクタ
	//-----------------------------------------------------
	Vertex::Vertex(Reader& reader, Header& header)
	{
		for (int i = 0; i < 4; i++) {
			m_boneIndex[i] = -1;
			m_boneWeight[i] = 0;
		}
		reader.read<float>(m_pos, 3);
		reader.read<float>(m_normal, 3);
		reader.read<float>(m_uv, 2);

		uint8_t bdef = reader.read<uint8_t>();
		switch (bdef) {
		case 0: // BDEF1
			m_boneIndex[0] = readBoneIndex(reader, header);
			m_boneWeight[0] = 1;
			break;
		case 1: // BDEF2
			m_boneIndex[0] = readBoneIndex(reader, header);
			m_boneIndex[1] = readBoneIndex(reader, header);
			m_boneWeight[0] = reader.read<float>();
			m_boneWeight[1] = 1 - m_boneWeight[0];
			break;
		case 2: // BDEF4
			m_boneIndex[0] = readBoneIndex(reader, header);
			m_boneIndex[1] = readBoneIndex(reader, header);
			m_boneIndex[2] = readBoneIndex(reader, header);
			m_boneIndex[3] = readBoneIndex(reader, header);
			m_boneWeight[0] = reader.read<float>();
			m_boneWeight[1] = reader.read<float>();
			m_boneWeight[2] = reader.read<float>();
			m_boneWeight[3] = reader.read<float>();
			break;
		case 3: // SDEF
			m_boneIndex[0] = readBoneIndex(reader, header);
			m_boneIndex[1] = readBoneIndex(reader, header);
			m_boneWeight[0] = reader.read<float>();
			m_boneWeight[1] = 1 - m_boneWeight[0];
			reader.skip(sizeof(float) * 9);
			break;
		default:
			FATAL(_T("Corrupted data\n"));
			return;
		}

		float w = 0;
		for (int i = 0; i < 4; i++) {
			w += m_boneWeight[i];
		}
		for (int i = 0; i < 4; i++) {
			m_boneWeight[i] /= w;
			if (m_boneIndex[i] == 84) {
				printf("aa");
			}
		}

		float edgeScale = reader.read<float>();
	}

	Face::Face(Reader& reader, Header& header)
	{
		uint8_t vertexIndexSize = header.vertexIndexSize();
		for (int32_t i = 0; i < 3; i++) {
			int32_t data;
			switch (vertexIndexSize) {
			case 1:
				data = reader.read<uint8_t>();
				break;
			case 2:
				data = reader.read<uint16_t>();
				break;
			case 4:
				data = reader.read<int32_t>();
				break;
			}
			if (data >= header.m_vertexCount) {
				FATAL(_T("Corrupted data\n"));
			}
			m_indices[i] = data;
		}
	}

	Material::Material(Reader& reader, Header& header)
	{
		std::string nameJ = readPmxString(reader, header.m_charEncoding);
		std::string nameE = readPmxString(reader, header.m_charEncoding);

		reader.read(m_diffuse, 4);
		reader.read(m_specular, 4);
		reader.read(m_ambient, 3);
		m_ambient[3] = 1.f;

		uint8_t m_drawFlags = reader.read<uint8_t>();
		reader.read(m_edgeColor, 4);
		reader.read(&m_edgeSize, 1);

		switch (header.textureIndexSize()) {
		case 1:
			m_textureIndex = reader.read<int8_t>();
			m_sphereTextureIndex = reader.read<uint8_t>();
			break;
		case 2:
			m_textureIndex = reader.read<int16_t>();
			m_sphereTextureIndex = reader.read<uint16_t>();
			break;
		case 4:
			m_textureIndex = reader.read<int32_t>();
			m_sphereTextureIndex = reader.read<int32_t>();
			break;
		}

		if (m_textureIndex != -1 && m_textureIndex >= header.m_textureCount) {
			FATAL(_T("Corrupted data\n"));
		}

		uint8_t m_sphereMode = reader.read<uint8_t>();
		uint8_t m_sharedToonFlag = reader.read<uint8_t>();
		if (m_sharedToonFlag) {
			uint8_t toonIndex = reader.read<uint8_t>();
		}
		else {
			reader.skip(header.textureIndexSize());
		}

		std::string note = readPmxString(reader, header.m_charEncoding);

		int32_t indexCount = reader.read<int32_t>();
		if (indexCount % 3) {
			FATAL(_T("Corrupted data\n"));
		}
		m_faceCount = indexCount / 3;
		m_faceIndex = header.m_materialFaceCount;
		header.m_materialFaceCount += m_faceCount;
	}

	IKLink::IKLink(Reader& reader, Header& header)
	{
		m_link = readBoneIndex(reader, header);
		uint8_t hasLimit = reader.read<uint8_t>();
		if (hasLimit) {
			reader.read<float>(m_lowerLimit, 3);
			reader.read<float>(m_upperLimit, 3);
		}
		else {
			const float kLimit = 3.15f;
			m_lowerLimit[0] = m_lowerLimit[1] = m_lowerLimit[2] = -kLimit;
			m_upperLimit[0] = m_upperLimit[1] = m_upperLimit[2] = kLimit;
		}
	}

	IKData::IKData(Reader& reader, Header& header)
	{
		m_target = readBoneIndex(reader, header);
		m_loopCount = reader.read<int32_t>();
		m_step = reader.read<float>();

		m_count = reader.read<int32_t>();
		for (int32_t i = 0; i < m_count; i++) {
			m_link.push_back(IKLink(reader, header));
		}
	}

	Bone::Bone(Reader& reader, Header& header)
	{
		std::string nameJ = readPmxString(reader, header.m_charEncoding);
		std::string nameE = readPmxString(reader, header.m_charEncoding);

		if (nameJ.length() >= sizeof m_name) {
			FATAL(_T("Unsupported Data\n"));
		}
		strncpy(m_name, nameJ.c_str(), sizeof m_name);

		reader.read<float>(m_pos, 3);
		m_parent = readBoneIndex(reader, header);
		m_phase = reader.read<int32_t>();
		m_flags = reader.read <uint16_t>();

		// 接続先 ()
		if (m_flags & 0x0001) {
			int32_t bi = readBoneIndex(reader, header);
		}
		else {
			float pos[3];
			reader.read<float>(pos, 3);
		}

		if (m_flags & 0x0300) {
			if ((m_flags & 0x0300) == 0x0300) {
				FATAL(_T("Corrupted data\n"));
			}
			m_effector = readBoneIndex(reader, header);
			m_effect = reader.read<float>();
		}
		else {
			m_effector = -1;
			m_effect = 0.0f;
		}

		if (m_flags & 0x0400) {
			float axis[3];
			reader.read<float>(axis, 3);
		}

		if (m_flags & 0x0800) {
			float axis0[3];
			float axis1[3];
			reader.read<float>(axis0, 3);
			reader.read<float>(axis1, 3);
		}

		if (m_flags & 0x2000) {
			int32_t key = reader.read<int32_t>();
		}

		if (m_flags & 0x0020) {
			int32_t id = header.m_ikData.size();
			header.m_ikData.push_back(IKData(reader, header));
			header.m_ikData.back().m_id = id;
			m_ik = id;
		}
		else {
			m_ik = -1;
		}
	}

	//-----------------------------------------------------
	// Info構造体
	//-----------------------------------------------------
	struct ModelInfo {
		ModelInfo(Reader& reader, const Header& header)
		{
			INFO(_T("============MODEL INFO============\n"));
			for (int i = 0; i < 4; i++) {
				m_info[i] = readPmxString(reader, header.m_charEncoding);
			}
		}
		std::string m_info[4];
	};

	struct VertexInfo {
		VertexInfo(Reader& reader, Header& header)
		{
			INFO(_T("============VERTEX============\n"));
			m_count = reader.read<int32_t>();
			header.m_vertexCount = m_count;
			INFO(_T("Count = %d\n"), m_count);

			switch (header.vertexIndexSize()) {
			case 1: if (m_count < 0 || m_count > 255) { FATAL(_T("Corrupted data\n")); } break;
			case 2: if (m_count < 256 || m_count > 65535) { FATAL(_T("Corrupted data\n")); } break;
			case 4: if (m_count < 65536 || m_count > 2147483647) { FATAL(_T("Corrupted data\n")); } break;
			}

			m_vertices.reserve(m_count);
			for (int32_t i = 0; i < m_count; i++) {
				m_vertices.push_back(Vertex(reader, header));
			}
		}

		int32_t m_count;
		std::vector<Vertex> m_vertices;
	};

	struct FaceInfo {
		FaceInfo(Reader& reader, Header& header)
		{
			INFO(_T("============FACE============\n"));
			m_count = reader.read<int32_t>();
			if (m_count < 0 || m_count % 3) {
				FATAL(_T("Corrupted data\n"));
			}
			m_count /= 3;
			header.m_faceCount = m_count;
			INFO(_T("Count = %d\n"), m_count);

			m_faces.reserve(m_count);
			for (int32_t i = 0; i < m_count; i++) {
				m_faces.push_back(Face(reader, header));
			}
		}

		int32_t m_count;
		std::vector<Face> m_faces;
	};

	struct Texture {
		Texture(Reader& reader, Header& header)
		{
			std::string str = readPmxString(reader, header.m_charEncoding);
			m_path = Util::toTstring(str);
		}
		Util::tstring m_path;
	};

	struct TextureInfo {
		TextureInfo(Reader& reader, Header& header)
		{
			INFO(_T("============TEXTURE============\n"));
			m_count = reader.read<int32_t>();
			header.m_textureCount = m_count;
			INFO(_T("Count = %d\n"), m_count);
			if (m_count < 0) {
				FATAL(_T("Corrupted data\n"));
			}

			for (int32_t i = 0; i < m_count; i++) {
				m_textures.push_back(Texture(reader, header));
			}
		}

		int32_t m_count;
		std::vector<Texture> m_textures;
	};

	struct MaterialInfo {
		MaterialInfo(Reader& reader, Header& header)
		{
			INFO(_T("============MATERIAL============\n"));
			m_count = reader.read<int32_t>();
			header.m_materialCount = m_count;
			INFO(_T("Count = %d\n"), m_count);
			if (m_count < 0) {
				FATAL(_T("Corrupted data\n"));
			}

			uint8_t textureIndexSize = header.textureIndexSize();

			for (int32_t i = 0; i < m_count; i++) {
				m_materials.push_back(Material(reader, header));
			}
		}

		int32_t m_count;
		std::vector<Material> m_materials;
	};

	struct BoneInfo {
		BoneInfo(Reader& reader, Header& header)
		{
			INFO(_T("============BONE============\n"));
			m_count = reader.read<int32_t>();
			header.m_boneCount = m_count;
			INFO(_T("Count = %d\n"), m_count);
			if (m_count < 0) {
				FATAL(_T("Corrupted data\n"));
			}

			for (int32_t i = 0; i < m_count; i++) {
				m_bones.push_back(Bone(reader, header));
			}
		}
		int32_t m_count;
		std::vector<Bone> m_bones;
	};

	struct VertexMorphHeader : public MOM::VertexMorphHeader {
		VertexMorphHeader(Reader& reader, Header& header, const std::string &nameJ, int32_t count)
		{
			if (nameJ.length() >= sizeof m_name) {
				FATAL(_T("Unsupported Data\n"));
			}
			strncpy(m_name, nameJ.c_str(), sizeof m_name);

			m_count = count;
			m_morphs.resize(count);
			for (int32_t i = 0; i < count; i++) {
				m_morphs[i].m_index = readVertexIndex(reader, header);
				reader.read<float>(m_morphs[i].m_offset, 3);
			}
		}
		std::vector<MOM::VertexMorphElem> m_morphs;
	};

	struct MorphInfo {
		MorphInfo(Reader& reader, Header& header)
		{
			INFO(_T("============MORPH============\n"));
			int32_t count = reader.read<int32_t>();
			if (count < 0) {
				FATAL(_T("Corrupted data\n"));
			}

			for (int32_t i = 0; i < count; i++) {
				auto nameJ = readPmxString(reader, header.m_charEncoding);
				auto nameE = readPmxString(reader, header.m_charEncoding);
				auto panel = reader.read<uint8_t>();
				auto type = reader.read<uint8_t>();

				int32_t count = reader.read <int32_t>();
				size_t elemSize = 0;
				switch (type) {
				case 0: // Group
					elemSize = header.morphIndexSize() + sizeof(float);
					reader.skip(count * elemSize);
					break;
				case 1: // Vertex
					m_vertexMorphs.push_back(VertexMorphHeader(reader, header, nameJ, count));
					break;
				case 2: // Bone
					elemSize = header.boneIndexSize() + sizeof(float) * 7;
					reader.skip(count * elemSize);
					break;
				case 3: // UV
					elemSize = header.vertexIndexSize() + sizeof(float) * 4;
					reader.skip(count * elemSize);
					break;
				case 4: // Additional UV
				case 5:
				case 6:
				case 7:
					elemSize = header.vertexIndexSize() + sizeof(float) * 4;
					reader.skip(count * elemSize);
					break;
				case 8:
					elemSize = header.materialIndexSize() + 1 + 16 + 12 + 4 + 12 + 16 + 4 + 16 + 16 + 16;
					reader.skip(count * elemSize);
					break;
				default:
					FATAL(_T("Corrupted Data\n"));
					break;
				}
			}
		}
		std::vector<VertexMorphHeader> m_vertexMorphs;
	};

	struct PmxInfo {
		PmxInfo(Reader& reader)
			: m_header(reader)
			, m_modelInfo(reader, m_header)
			, m_vertexInfo(reader, m_header)
			, m_faceInfo(reader, m_header)
			, m_textureInfo(reader, m_header)
			, m_materialInfo(reader, m_header)
			, m_boneInfo(reader, m_header)
			, m_morphInfo(reader, m_header)
		{
		}

		Header m_header;
		ModelInfo m_modelInfo;
		VertexInfo m_vertexInfo;
		FaceInfo m_faceInfo;
		TextureInfo m_textureInfo;
		MaterialInfo m_materialInfo;
		BoneInfo m_boneInfo;
		MorphInfo m_morphInfo;
	};

} // pmx

namespace MOM
{

	// Convert
	InfoHeader fromPmx(const pmx::Header& pmx)
	{
		InfoHeader mom = {};

		mom.m_vertexCount = pmx.m_vertexCount;
		mom.m_faceCount = pmx.m_faceCount;
		mom.m_textureCount = pmx.m_textureCount;
		mom.m_materialCount = pmx.m_materialCount;
		mom.m_boneCount = pmx.m_boneCount;

		return mom;
	}

	static void writeChunkHeader(FILE* fp, uint32_t type, uint32_t index, uint32_t contentSize)
	{
		ChunkHeader header;
		header.m_type = type;
		header.m_index = index;
		header.m_contentSize = contentSize;
		fwrite(&header, sizeof(ChunkHeader), 1, fp);
	}

	static void writeChunk(FILE* fp, uint32_t type, uint32_t index, uint32_t size, const void* data)
	{
		uint32_t contentSize = (size + 3) & ~3;
		writeChunkHeader(fp, type, index, contentSize);
		fwrite(data, 1, size, fp);
		if (contentSize > size) {
			uint8_t padding[4] = {};
			fwrite(padding, 1, contentSize - size, fp);
		}
	}

	template <typename T>
	void writeArray(const std::vector<T> &array, FILE* fp)
	{
		size_t count = array.size();
		for (size_t i = 0; i < count; i++) {
			if (fwrite(&array[i], sizeof(T::BaseType), 1, fp) != 1) {
				throw;
			}
		}
	}

	static void appendIKData(const pmx::IKData& ikData, FILE* fp)
	{
		size_t n = ikData.m_link.size();
		uint32_t contentSize = sizeof(MOM::IKData) + n * sizeof(MOM::IKLink);
		writeChunkHeader(fp, CHUNK_TYPE_IKDT, ikData.m_id, contentSize);
		fwrite(&ikData, sizeof(MOM::IKData), 1, fp);
		writeArray(ikData.m_link, fp);
	}

	static void appendVertexMorph(const pmx::VertexMorphHeader& vm, uint32_t index, FILE* fp)
	{
		size_t count = vm.m_count;
		uint32_t contentSize = sizeof(MOM::VertexMorphHeader) + count * sizeof(MOM::VertexMorphElem);
		writeChunkHeader(fp, CHUNK_TYPE_VMOR, index, contentSize);

		fwrite(&vm, sizeof(MOM::VertexMorphHeader), 1, fp);
		fwrite(&vm.m_morphs[0], sizeof(MOM::VertexMorphElem), count, fp);
	}

	static void appendTextureData(const pmx::Texture& tex, uint32_t index, FILE* fp)
	{
		Util::tstring input = tex.m_path;

		Util::tstring output;
		output += input;
		output = output.substr(0, output.rfind('.'));
		output += _T(".dds");


		{
			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			memset(&si, 0, sizeof(si));
			si.cb = sizeof(si);

			_TCHAR currrentDir[MAX_PATH];
			::GetCurrentDirectory(MAX_PATH, currrentDir);

			tstring commandLine = s_texconvPath;
			commandLine += _T(" -f DXT3");
			commandLine += _T(" ");
			commandLine += input;

			size_t bufSize = commandLine.length() + 1;
			_TCHAR* buf = new _TCHAR[bufSize];
			_tcsncpy_s(buf, bufSize, commandLine.c_str(), bufSize - 1);

			if (::CreateProcess(NULL, buf, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, currrentDir, &si, &pi)) {
				::WaitForSingleObject(pi.hProcess, INFINITE);
				::CloseHandle(pi.hProcess);
			}
			delete[] buf;
		}

		HANDLE hFile = ::CreateFile(output.c_str(), GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			uint32_t size = 0;
			writeChunkHeader(fp, CHUNK_TYPE_XDDS, index, 0);
			return;
		}

		DWORD fileSize = 0;
		fileSize = ::GetFileSize(hFile, NULL);
		uint32_t size = (fileSize + 3) & ~3;
		uint8_t *data = new uint8_t[size];
		memset(data, 0, size);
		::ReadFile(hFile, data, fileSize, &fileSize, NULL);
		::CloseHandle(hFile);
		writeChunk(fp, CHUNK_TYPE_XDDS, index, size, data);
		delete[] data;
		return;
	}

	void pmx2mom(const pmx::PmxInfo& pmx, FILE* fp)
	{
		FileHeader fh = { kMagic, kVersion };
		fwrite(&fh, sizeof(FileHeader), 1, fp);

		// InfoHeader
		InfoHeader header = fromPmx(pmx.m_header);
		writeChunkHeader(fp, CHUNK_TYPE_INFO, 0, sizeof(InfoHeader));
		fwrite(&header, sizeof(InfoHeader), 1, fp);

		// Vertex
		writeChunkHeader(fp, CHUNK_TYPE_VERT, 0, sizeof(Vertex)* header.m_vertexCount);
		writeArray(pmx.m_vertexInfo.m_vertices, fp);

		// Face
		writeChunkHeader(fp, CHUNK_TYPE_FACE, 0, sizeof(Face)* header.m_faceCount);
		writeArray(pmx.m_faceInfo.m_faces, fp);

		// Material
		writeChunkHeader(fp, CHUNK_TYPE_MATE, 0, sizeof(Material)* header.m_materialCount);
		writeArray(pmx.m_materialInfo.m_materials, fp);

		// Bone
		writeChunkHeader(fp, CHUNK_TYPE_BONE, 0, sizeof(Bone)* header.m_boneCount);
		writeArray(pmx.m_boneInfo.m_bones, fp);

		// IKData
		for (size_t i = 0; i < pmx.m_header.m_ikData.size(); i++) {
			appendIKData(pmx.m_header.m_ikData[i], fp);
		}

		// VertexMorph
		for (size_t i = 0; i < pmx.m_morphInfo.m_vertexMorphs.size(); i++) {
			appendVertexMorph(pmx.m_morphInfo.m_vertexMorphs[i], i, fp);
		}

		// Texture
		for (int32_t i = 0; i < header.m_textureCount; i++) {
			appendTextureData(pmx.m_textureInfo.m_textures[i], i, fp);
		}

		// Comments
		int32_t commIdx = 0;
		for (int32_t i = 0; i < 4; i++) {
			if (!pmx.m_modelInfo.m_info[i].empty()) {
				writeChunk(fp, CHUNK_TYPE_COMM, commIdx++, pmx.m_modelInfo.m_info[i].length(), pmx.m_modelInfo.m_info[i].c_str());
			}
		}
	}

} // MOM

int convert(int argc, _TCHAR* argv[])
{
	// Texconv を探す
	{
		_TCHAR dxsdkDir[MAX_PATH] = _T("");
		DWORD len = GetEnvironmentVariable(_T("DXSDK_DIR"), dxsdkDir, MAX_PATH);
		if (len == 0) {
			FATAL(_T("DXSDK_DIR is not set\n"));
		}
		Util::tstring path(dxsdkDir, len);
		path += _T("Utilities\\bin\\x86\\texconv.exe");
		s_texconvPath = path;

		// チェック
		HINSTANCE hInst = ShellExecute(NULL, _T("open"), s_texconvPath.c_str(), NULL, NULL, SW_HIDE);
		if ((int)hInst <= 32) {
			FATAL(_T("Cannot open %s\n"), s_texconvPath.c_str());
		}
	}

	Reader reader(argv[1]);
	pmx::PmxInfo pmxInfo(reader);

	Util::tstring baseDir = getBaseDir(argv[1]);
	SetCurrentDirectory(baseDir.c_str());

	tstring outFile = getFname(argv[1]);
	outFile += _T(".mom");

	FILE* fout;
	if (_tfopen_s(&fout, outFile.c_str(), _T("wb"))) {
		FATAL(_T("Cannot open %s\n"), argv[2]);
	}
	MOM::pmx2mom(pmxInfo, fout);
	fclose(fout);

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1) {
		INFO(_T("pmx2mom version 0x%08X"), MOM::kVersion);
		Sleep(3 * 1000);
		return -1;
	}
	return convert(argc, argv);
}
