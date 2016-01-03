//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include <MOEGUL/GLUtil.h>

#include "debug.h"

namespace MOEGUL
{
	Program buildShaderProgram(const char* vsSource, const char* fsSource)
	{
		GL& gl = GL::getInstance();
		auto vs = gl.createShader(gl.VERTEX_SHADER);
		auto fs = gl.createShader(gl.FRAGMENT_SHADER);
		if (!vs || !fs) {
			// Failed to create shader instances.
			return Program();
		}

		// Compile vertex shader
		gl.shaderSource(vs, vsSource);
		gl.compileShader(vs);
		bool success = (bool)gl.getShaderParameter(vs, gl.COMPILE_STATUS);
		if (!success) {
			std::string log = gl.getShaderInfoLog(vs);
			DBG_ERROR("%s\n", log.c_str());
			return Program();
		}
		// Compile fragment shader
		gl.shaderSource(fs, fsSource);
		gl.compileShader(fs);
		success = (bool)gl.getShaderParameter(fs, gl.COMPILE_STATUS);
		if (!success) {
			std::string log = gl.getShaderInfoLog(fs);
			DBG_ERROR("%s\n", log.c_str());
			return Program();
		}

		// Link program
		Program prog = gl.createProgram();
		if (!prog) {
			return Program();
		}
		gl.attachShader(prog, vs);
		gl.attachShader(prog, fs);
		gl.linkProgram(prog);
		success = (bool)gl.getProgramParameter(prog, gl.LINK_STATUS);
		if (!success) {
			std::string log = gl.getProgramInfoLog(prog);
			DBG_ERROR("%s\n", log.c_str());
			return Program();
		}

		// All OK
		return prog;
	}

	Buffer buildVertexBuffer(const void* data, size_t size)
	{
		if (!data || size == 0) {
			return Buffer();
		}

		GL& gl = GL::getInstance();
		// バッファを生成
		Buffer buffer = gl.createBuffer();
		// ARRAY_BUFFERにバインド
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		if (gl.getError()) {
			return Buffer();
		}

		gl.bufferData(gl.ARRAY_BUFFER, size, data, gl.STATIC_DRAW);
		if (gl.getError()) {
			return Buffer();
		}

		return buffer;
	}

	Buffer buildIndexBuffer(const void* data, size_t size)
	{
		if (!data || size == 0) {
			return Buffer();
		}

		GL& gl = GL::getInstance();
		// バッファを生成
		Buffer buffer = gl.createBuffer();
		// ELEMENT_ARRAY_BUFFERにバインド
		gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
		if (gl.getError()) {
			return Buffer();
		}

		gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, size, data, gl.STATIC_DRAW);
		if (gl.getError()) {
			return Buffer();
		}

		return buffer;
	}

	Texture loadTexture(const void* data, size_t size)
	{
		// フォーマットを判別する
		if (size < sizeof(uint32_t)) {
			return Texture();
		}
		uint32_t magic;
		memcpy(&magic, data, sizeof magic);
		if (magic == 0x20534444) {
			// DDS
			return loadDDSTexture(data, size);
		}

		// ロード失敗
		return Texture();
	}

	Texture loadDDSTexture(const void* data, size_t size)
	{
		struct DDSHeader {
			uint32_t magic;
			uint32_t size;
			uint32_t flags;
			uint32_t height;
			uint32_t width;
			uint32_t pitch;
			uint32_t depth;
			uint32_t mipMapConut;
			uint32_t reserved[11];
			uint32_t pfSize;
			uint32_t pfFlags;
			uint32_t fourCC;
			uint32_t rgbBitCount;
			uint32_t rBitMask;
			uint32_t gBitMask;
			uint32_t bBitMask;
			uint32_t aBitMask;
			uint32_t reserved2[5];
		};
		DDSHeader header;

		if (size < sizeof(header)) {
			return Texture();
		}

		const uint8_t* src = (const uint8_t*)data;
		memcpy(&header, src, sizeof(header));
		src += sizeof(header);
		size -= sizeof(header);

		// Check format
		if (header.magic != 0x20534444 || header.size != 0x7C || header.pfSize != 0x20) {
			return Texture();
		}

		GL& gl = GL::getInstance();
		uint32_t format;
		switch (header.fourCC) {
		case 0x33545844: // DXT3
			format = gl.COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		default:
			// Unsupported
			return Texture();
		}

		// Generate texture
		Texture texture = gl.createTexture();
		
		gl.bindTexture(gl.TEXTURE_2D, texture);
		if (gl.getError()) {
			return Texture();
		}
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);

		uint32_t width = header.width;
		uint32_t height = header.height;
		for (uint32_t i = 0; i < header.mipMapConut; i++) {
			// 最少ブロックは4x4
			uint32_t texSize = ((width + 3) & ~3) * ((height + 3) & ~3);
			if (size < texSize) {
				return Texture();
			}

			gl.compressedTexImage2D(gl.TEXTURE_2D, i, format, width, height, 0, texSize, src);
			if (gl.getError()) {
				return Texture();
			}

			src += texSize;
			size -= texSize;
			width /= 2;
			height /= 2;
			if (width == 0) {
				width = 1;
			}
			if (height == 0) {
				width = 1;
			}
		}

		return texture;
	}
}
