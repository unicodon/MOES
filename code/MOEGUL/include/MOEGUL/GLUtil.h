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
	/// ShaderProgramをcompile & link する
	/// @param vsSource [in] Vertex shader 文字列
	/// @param fsSource [in] Fragment shader 文字列
	Program buildShaderProgram(const char* vsSource, const char* fsSource);

	/// 頂点用のバッファを生成する
	Buffer buildVertexBuffer(const void* data, size_t size);

	/// Index用のバッファを生成する
	Buffer buildIndexBuffer(const void* data, size_t size);

	/// Texture をロードする。
	/// @note 内部でフォーマット判別を行う
	Texture loadTexture(const void* data, size_t size);

	/// DDS Texture をロードする。<br>
	/// 対応フォーマット: DDS(DXT3)
	Texture loadDDSTexture(const void* data, size_t size);
}
