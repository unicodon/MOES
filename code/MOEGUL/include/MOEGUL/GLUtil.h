//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include "GL.h"

namespace MOEGUL
{
	/// ShaderProgram��compile & link ����
	/// @param vsSource [in] Vertex shader ������
	/// @param fsSource [in] Fragment shader ������
	Program buildShaderProgram(const char* vsSource, const char* fsSource);

	/// ���_�p�̃o�b�t�@�𐶐�����
	Buffer buildVertexBuffer(const void* data, size_t size);

	/// Index�p�̃o�b�t�@�𐶐�����
	Buffer buildIndexBuffer(const void* data, size_t size);

	/// Texture �����[�h����B
	/// @note �����Ńt�H�[�}�b�g���ʂ��s��
	Texture loadTexture(const void* data, size_t size);

	/// DDS Texture �����[�h����B<br>
	/// �Ή��t�H�[�}�b�g: DDS(DXT3)
	Texture loadDDSTexture(const void* data, size_t size);
}
