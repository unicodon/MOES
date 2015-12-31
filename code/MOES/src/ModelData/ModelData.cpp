//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "ModelData.h"

#include <map>
#include <set>
#include <JAFFile.h>
#include <JTL.h>
#include <MOEGUL/GLUtil.h>
#include <MOMData.h>

#include "debug.h"

/// value == expect であることをチェック
#define CHECK_EQ_INT(value, expect) \
	if ((value) != (expect)) { \
		DBG_ERROR("Error: %s (%08x) != %s (%08x)\n", #value, (value), #expect, (expect)); \
		return 0; \
	}

/// value >= 0 であることをチェック
#define CHECK_GE_ZERO(value) \
	if ((value) < 0) { \
		DBG_ERROR("Error: %s (%08x) 0\n", #value, (value)); \
		return 0; \
	}

namespace MOES
{
	ModelData::ModelData()
	{

	}

	ModelData::~ModelData()
	{

	}

	ModelData::Ptr ModelData::create(FILE *fp)
	{
		Ptr ptr(new ModelData);
		if (!ptr) {
			return Ptr();
		}

		int readCount;

		MOM::FileHeader fh;
		readCount = JAF::fread(&fh, sizeof(fh), 1, fp);
		CHECK_EQ_INT(readCount, 1);
		CHECK_EQ_INT(fh.m_magic, MOM::kMagic);
		CHECK_EQ_INT(fh.m_version, MOM::kVersion);

		//-------------------------------------
		// InfoHeader をロード
		//-------------------------------------
		MOM::ChunkHeader ch;
		readCount = JAF::fread(&ch, sizeof(ch), 1, fp);
		CHECK_EQ_INT(ch.m_type, MOM::CHUNK_TYPE_INFO);
		CHECK_EQ_INT(ch.m_contentSize, sizeof(MOM::InfoHeader));
		CHECK_EQ_INT(ch.m_index, 0);
		MOM::InfoHeader header;
		readCount = JAF::fread(&header, sizeof(header), 1, fp);
		CHECK_EQ_INT(readCount, 1);
		// InfoHeader のチェック
		CHECK_GE_ZERO(header.m_vertexCount);
		CHECK_GE_ZERO(header.m_faceCount);
		CHECK_GE_ZERO(header.m_materialCount);
		CHECK_GE_ZERO(header.m_boneCount);

		//-------------------------------------
		// 頂点データのロード
		//-------------------------------------
		readCount = JAF::fread(&ch, sizeof(ch), 1, fp);
		CHECK_EQ_INT(readCount, 1);
		CHECK_EQ_INT(ch.m_type, MOM::CHUNK_TYPE_VERT);
		CHECK_EQ_INT(ch.m_contentSize, header.m_vertexCount * sizeof(MOM::Vertex));
		CHECK_EQ_INT(ch.m_index, 0);
		std::vector<Vertex> vertices;
		{
			int count = header.m_vertexCount;
			std::vector<MOM::Vertex> data;
			data.resize(count);
			vertices.resize(count);
			readCount = JAF::fread(&data[0], sizeof(MOM::Vertex), count, fp);
			CHECK_EQ_INT(readCount, count);
			for (int i = 0; i < count; ++i) {
				memcpy(vertices[i].m_pos, data[i].m_pos, sizeof vertices[i].m_pos);
				memcpy(vertices[i].m_normal, data[i].m_normal, sizeof vertices[i].m_normal);
				memcpy(vertices[i].m_uv, data[i].m_uv, sizeof vertices[i].m_uv);
				memcpy(vertices[i].m_weight, data[i].m_boneWeight, sizeof vertices[i].m_weight);
				for (int l = 0; l < 4; l++) {
					vertices[i].m_bone[l] = data[i].m_boneIndex[l];
				}
			}
			ptr->m_vertexCount = count;
		}

		//-------------------------------------
		// 面データのロード
		//-------------------------------------
		readCount = JAF::fread(&ch, sizeof(ch), 1, fp);
		CHECK_EQ_INT(readCount, 1);
		CHECK_EQ_INT(ch.m_type, MOM::CHUNK_TYPE_FACE);
		CHECK_EQ_INT(ch.m_contentSize, header.m_faceCount * sizeof(MOM::Face));
		CHECK_EQ_INT(ch.m_index, 0);
		std::vector<MOM::Face> faces;
		{
			int count = header.m_faceCount;
			faces.resize(count);
			readCount = JAF::fread(&faces[0], sizeof(MOM::Face), count, fp);
			CHECK_EQ_INT(readCount, count);
			ptr->m_faceCount = count;
		}

		//-------------------------------------
		// Mesh の ロード
		//-------------------------------------
		readCount = JAF::fread(&ch, sizeof(ch), 1, fp);
		CHECK_EQ_INT(readCount, 1);
		CHECK_EQ_INT(ch.m_type, MOM::CHUNK_TYPE_MATE);
		CHECK_EQ_INT(ch.m_contentSize, header.m_materialCount * sizeof(MOM::Material));
		CHECK_EQ_INT(ch.m_index, 0);
		{
			int count = header.m_materialCount;
			std::vector<MOM::Material> materialData;
			materialData.resize(count);
			readCount = JAF::fread(&materialData[0], sizeof(MOM::Material), count, fp);
			CHECK_EQ_INT(readCount, count);

			for (int i = 0; i < count; i++) {
				auto& material = materialData[i];
				Mesh mesh;
				mesh.m_material.m_textureIdx = material.m_textureIndex;
				memcpy(mesh.m_material.m_diffuse, material.m_diffuse, sizeof mesh.m_material.m_diffuse);
				memcpy(mesh.m_material.m_specular, material.m_specular, sizeof mesh.m_material.m_specular);
				memcpy(mesh.m_material.m_ambient, material.m_ambient, sizeof mesh.m_material.m_ambient);

				// 参照ボーン数がMesh::kMaxMatricesを超えないようにサブメッシュに分割
				struct FaceRange {
					int32_t first;
					int32_t last;
				};
				FaceRange range;
				std::vector<FaceRange> ranges;
				std::set<int32_t> boneSet;
				int32_t faceIdx = material.m_faceIndex;
				int32_t lastIdx = material.m_faceIndex + material.m_faceCount;
				range.first = material.m_faceIndex;
				for (; faceIdx < lastIdx; faceIdx++) {
					auto& face = faces[faceIdx];
					for (int k = 0; k < 3; k++) {
						for (int l = 0; l < 4; l++) {
							int32_t boneIdx = vertices[face.m_indices[k]].m_bone[l];
							if (vertices[face.m_indices[k]].m_weight[l] > 0 && boneIdx >= 0) {
								boneSet.insert(boneIdx);
							}
						}
					}

					// 参照ボーン数が制限を超えた
					if (boneSet.size() > Mesh::kMaxMatrices) {
						DBG_INFO("Submesh %d / %d\n", faceIdx, lastIdx);
						range.last = faceIdx;
						ranges.push_back(range);

						// 次のサブメッシュ範囲を開始
						boneSet.clear();
						range.first = faceIdx;
						// もう一度今のFaceを処理するためにIndexを巻き戻す
						faceIdx--;
						continue;
					}
				}
				// 最後のサブメッシュ範囲を追加
				range.last = lastIdx;
				ranges.push_back(range);

				// サブメッシュを構築する
				for (auto& range : ranges) {
					memset(mesh.m_bones, 0, sizeof mesh.m_bones);

					// 全体のvertexId からサブメッシュのVertexIdへのマップ
					std::map<int32_t, int32_t> vertexIdMap;
					// 全体のBoneIdからサブメッシュのBoneIdへのマップ
					std::map<int32_t, int32_t> boneIdMap;

					std::vector<Vertex> subVertices;
					std::vector<int32_t> subIndices;
					std::vector<int32_t> originalIndices;
					for (int32_t idx = range.first; idx < range.last; idx++) {
						for (int k = 0; k < 3; k++) {
							int32_t vertexId = faces[idx].m_indices[k];
							auto it = vertexIdMap.find(vertexId);
							if (it == vertexIdMap.end()) {
								int32_t newId = subVertices.size();
								it = vertexIdMap.insert(it, std::make_pair(vertexId, newId));

								// Vertex をサブメッシュに追加する
								Vertex vertex = vertices[vertexId];
								// がその前にBoneIDをサブメッシュローカルに変換する
								for (int l = 0; l < 4; l++) {
									if (vertex.m_bone[l] < 0) {
										vertex.m_bone[l] = 0;
										vertex.m_weight[l] = 0.0f;
									}
									if (vertex.m_weight[l] > 0) {
										// グローバルなBoneID
										int32_t boneId = vertex.m_bone[l];
										auto bit = boneIdMap.find(boneId);
										if (bit == boneIdMap.end()) {
											// ボーンをマップに新規追加
											int32_t newBoneId = boneIdMap.size();
											bit = boneIdMap.insert(bit, std::make_pair(boneId, newBoneId));
											// パレット用のボーンリストにも追加
											mesh.m_bones[newBoneId] = boneId;
										}
										vertex.m_bone[l] = bit->second;
									}
									else {
										vertex.m_bone[l] = 0;
									}
								}

								// 今度こそVertexを追加する
								subVertices.push_back(vertex);
								originalIndices.push_back(vertexId);
							}
							subIndices.push_back(it->second);
						}
					}
					mesh.m_vbo = MOEGUL::buildVertexBuffer(&subVertices[0], subVertices.size() * sizeof(Vertex));
					mesh.m_ibo = MOEGUL::buildIndexBuffer(&subIndices[0], subIndices.size() * sizeof(int32_t));
					mesh.m_indexCount = subIndices.size();
					mesh.m_originalVertexIndices.swap(originalIndices);
					ptr->m_meshes.push_back(mesh);
				}
			}
		}

		//-------------------------------------
		// Bone の ロード
		//-------------------------------------
		readCount = JAF::fread(&ch, sizeof(ch), 1, fp);
		CHECK_EQ_INT(readCount, 1);
		CHECK_EQ_INT(ch.m_type, MOM::CHUNK_TYPE_BONE);
		CHECK_EQ_INT(ch.m_contentSize, header.m_boneCount * sizeof(MOM::Bone));
		CHECK_EQ_INT(ch.m_index, 0);
		{
			int count = header.m_boneCount;
			std::vector<MOM::Bone> boneData;
			boneData.resize(count);
			ptr->m_bones.resize(count);
			readCount = JAF::fread(&boneData[0], sizeof(MOM::Bone), count, fp);
			CHECK_EQ_INT(readCount, count);
			for (int i = 0; i < count; i++) {
				auto& bone = ptr->m_bones[i];

				bone.m_idx = i;
				bone.m_parentIdx = boneData[i].m_parent;
				bone.m_ikTargetIdx = boneData[i].m_ik;
				memcpy(bone.m_pos, boneData[i].m_pos, sizeof(float) * 3);

				bone.m_phase = boneData[i].m_phase;
				bone.m_effectorIdx = boneData[i].m_effector;
				bone.m_effect = boneData[i].m_effect;

				bone.m_posEffect = (boneData[i].m_flags & MOM::Bone::kBoneFlagPosEffect) != 0;
				bone.m_rotEffect = (boneData[i].m_flags & MOM::Bone::kBoneFlagRotEffect) != 0;
				bone.m_afterPhys = (boneData[i].m_flags & MOM::Bone::kBoneFlagAfterPhys) != 0;

				ptr->m_boneNames.add(boneData[i].m_name, i);
			}
		}

		// Load Raw Chunk
		while (JAF::fread(&ch, sizeof(ch), 1, fp) == 1) {
			// Read contents
			void* chunkData = malloc(ch.m_contentSize);
			if (ch.m_contentSize > 0) {
				readCount = JAF::fread(chunkData, 1, ch.m_contentSize, fp);
				CHECK_EQ_INT(readCount, ch.m_contentSize);
			}

			if (ch.m_type == MOM::CHUNK_TYPE_XDDS) {
				ptr->m_textures.push_back(MOEGUL::loadDDSTexture(chunkData, ch.m_contentSize));
			}
			else if (ch.m_type == MOM::CHUNK_TYPE_IKDT) {
				MOM::IKData* data = (MOM::IKData*)chunkData;
				MOM::IKLink* link = (MOM::IKLink*)(data + 1);
				ptr->m_ikBones.push_back(IKBone());

				IKBone& ikBone = ptr->m_ikBones.back();
				ikBone.m_id = data->m_id;
				ikBone.m_target = data->m_target;
				ikBone.m_loopCount = data->m_loopCount;
				ikBone.m_step = data->m_step;
				ikBone.m_link.resize(data->m_count);
				for (int i = 0; i < data->m_count; i++) {
					ikBone.m_link[i].m_link = link->m_link;
					memcpy(ikBone.m_link[i].m_lowerLimit, link->m_lowerLimit, sizeof(float) * 3);
					memcpy(ikBone.m_link[i].m_upperLimit, link->m_upperLimit, sizeof(float) * 3);
					link++;
				}
			}
			else if (ch.m_type == MOM::CHUNK_TYPE_VMOR) {
				MOM::VertexMorphHeader* vmh = (MOM::VertexMorphHeader*)chunkData;
				MOM::VertexMorphElem* elem = (MOM::VertexMorphElem*)(vmh + 1);
				// 既に同名のVMORが登録済みの場合は何もしない。
				if (ptr->m_vertexMorphNames.lookUp(vmh->m_name) < 0) {
					ptr->m_vertexMorphNames.add(vmh->m_name, ptr->m_vertexMorphs.size());
					ptr->m_vertexMorphs.push_back(VertexMorph());
					auto &vm = ptr->m_vertexMorphs.back();
					vm.m_elems.resize(vmh->m_count);
					for (int i = 0; i < vmh->m_count; i++) {
						vm.m_elems[i].m_idx = elem->m_index;
						vm.m_elems[i].m_offset[0] = elem->m_offset[0];
						vm.m_elems[i].m_offset[1] = elem->m_offset[1];
						vm.m_elems[i].m_offset[2] = elem->m_offset[2];
						elem++;
					}
				}
			}
			else if (ch.m_type == MOM::CHUNK_TYPE_COMM) {
				if (ch.m_contentSize > 0) {
					std::string str((char*)chunkData, ch.m_contentSize);
					printf("%s\n", str.c_str());
				}
			}

			free(chunkData);
		}

		// テクスチャの解決をここでしておく
		for (auto& mesh : ptr->m_meshes) {
			int32_t textureIdx = mesh.m_material.m_textureIdx;
			if (textureIdx < ptr->m_textures.size()) {
				mesh.m_tex = ptr->m_textures[textureIdx];
			}
		}

		return ptr;
	}
}
