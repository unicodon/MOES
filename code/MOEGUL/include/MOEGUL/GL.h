//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include "GLTypes.h"

namespace MOEGUL
{
	//-------------------------------------
	// GL object 定義
	//-------------------------------------
	DEF_GLOBJ(Buffer);
	DEF_GLOBJ(Shader);
	DEF_GLOBJ(Program);
	DEF_GLOBJ(Texture);

	/// OpenGL APIのラッパークラス。
	class GL {
	public:
		//-------------------------------------
		// 定数
		//-------------------------------------
		enum {
			DEPTH_BUFFER_BIT = 0x00000100,
			STENCIL_BUFFER_BIT = 0x00000400,
			COLOR_BUFFER_BIT = 0x00004000,

			POINTS = 0x0000,
			LINES = 0x0001,
			LINE_LOOP = 0x0002,
			LINE_STRIP = 0x0003,
			TRIANGLES = 0x0004,
			TRIANGLE_STRIP = 0x0005,
			TRIANGLE_FAN = 0x0006,

			ZERO = 0,
			ONE = 1,
			SRC_COLOR = 0x0300,
			ONE_MINUS_SRC_COLOR = 0x0301,
			SRC_ALPHA = 0x0302,
			ONE_MINUS_SRC_ALPHA = 0x0303,
			DST_ALPHA = 0x0304,
			ONE_MINUS_DST_ALPHA = 0x0305,
			DST_COLOR = 0x0306,
			ONE_MINUS_DST_COLOR = 0x0307,
			SRC_ALPHA_SATURATE = 0x0308,

			FRONT = 0x0404,
			BACK = 0x0405,

			CW = 0x0900,
			CCW = 0x0901,

			CULL_FACE = 0x0B44,
			DEPTH_TEST = 0x0B71,
			STENCIL_TEST = 0x0B90,
			BLEND = 0x0BE2,
			SCISSOR_TEST = 0x0C11,

			TEXTURE_2D = 0x0DE1,

			BYTE = 0x1400,
			UNSIGNED_BYTE = 0x1401,
			SHORT = 0x1402,
			UNSIGNED_SHORT = 0x1403,
			INT = 0x1404,
			UNSIGNED_INT = 0x1405,
			FLOAT = 0x1406,

			ALPHA = 0x1906,
			RGB = 0x1907,
			RGBA = 0x1908,
			LUMINANCE = 0x1909,
			LUMINANCE_ALPHA = 0x190A,

			NEAREST = 0x2600,
			LINEAR = 0x2601,
			NEAREST_MIPMAP_NEARES = 0x2700,
			LINEAR_MIPMAP_NEAREST = 0x2701,
			NEAREST_MIPMAP_LINEAR = 0x2702,
			LINEAR_MIPMAP_LINEAR = 0x2703,

			TEXTURE_MAG_FILTER = 0x2800,
			TEXTURE_MIN_FILTER = 0x2801,
			TEXTURE_WRAP_S = 0x2802,
			TEXTURE_WRAP_T = 0x2803,

			ARRAY_BUFFER = 0x8892,
			ELEMENT_ARRAY_BUFFER = 0x8893,

			STREAM_DRAW = 0x88E0,
			STATIC_DRAW = 0x88E4,
			DYNAMIC_DRAW = 0x88E8,

			FRAGMENT_SHADER = 0x8B30,
			VERTEX_SHADER = 0x8B31,
			COMPILE_STATUS = 0x8B81,
			LINK_STATUS = 0x8B82,
			INFO_LOG_LENGTH = 0x8B84,

			// Error code
			INVALID_ENUM = 0x0500,
			INVALID_VALUE = 0x0501,
			INVALID_OPERATION = 0x0502,
			OUT_OF_MEMORY = 0x0505,

			// EXT
			COMPRESSED_RGB_S3TC_DXT1_EXT = 0x83F0,
			COMPRESSED_RGBA_S3TC_DXT1_EXT = 0x83F1,
			COMPRESSED_RGBA_S3TC_DXT3_EXT = 0x83F2,
			COMPRESSED_RGBA_S3TC_DXT5_EXT = 0x83F3,

			RGBA32F_EXT = 0x8814,
		};

		//-------------------------------------
		// GL API (in alphabetical order)
		//-------------------------------------
		void		attachShader(Program, Shader);
		void		bindBuffer(uint32_t target, Buffer);
		void		bindTexture(uint32_t target, Texture);
		void		blendFunc(uint32_t sfactor, uint32_t dfactor);
		void		bufferData(uint32_t target, uint32_t size, const void* data, uint32_t usage);
		void		clear(uint32_t mask);
		void		clearColor(float red, float green, float blue, float alpha);
		void		compileShader(Shader);
		void		compressedTexImage2D(uint32_t target, int32_t level, uint32_t internalformat, int32_t width, int32_t height, int32_t border, int32_t imageSize, const void *data);
		Buffer		createBuffer();
		Program		createProgram();
		Shader		createShader(uint32_t type);
		Texture		createTexture();		
		void		cullFace(uint32_t mode);
		void		disable(uint32_t cap);
		void		disableVertexAttribArray(uint32_t idx);
		void		drawArrays(uint32_t mode, int32_t first, int32_t count);
		void		drawElements(uint32_t mode, int32_t count, uint32_t type, void* offset);
		void		enable(uint32_t cap);
		void		enableVertexAttribArray(uint32_t idx);
		void		frontFace(uint32_t mode);
		uint32_t	getError();
		int32_t		getAttribLocation(Program, const char* name);
		std::string getProgramInfoLog(Program);
		int32_t		getProgramParameter(Program, uint32_t pname);
		std::string getShaderInfoLog(Shader);
		int32_t		getShaderParameter(Shader, uint32_t pname);
		int32_t		getUniformLocation(Program, const char* name);
		void		linkProgram(Program);
		void		shaderSource(Shader, const char* source);
		void		texImage2D(uint32_t target, int32_t level, uint32_t internalformat, int32_t width, int32_t height, int32_t border, uint32_t format, int32_t type, const void *pixels);
		void		texParameterf(uint32_t target, uint32_t pname, float param);
		void		texParameterfv(uint32_t target, uint32_t pname, const float* params);
		void		texParameteri(uint32_t target, uint32_t pname, int32_t param);
		void		texParameteriv(uint32_t target, uint32_t pname, const int32_t* params);
		void		useProgram(Program);
		void		vertexAttribPointer(uint32_t idx, int32_t size, uint32_t type, bool normalized, int32_t stride, void* offset);
		void		viewport(int32_t x, int32_t y, int32_t width, int32_t height);

		//-------------------------------------
		// GL API (uniform)
		//-------------------------------------
		void		uniform1f(int32_t location, float v0);
		void		uniform2f(int32_t location, float v0, float v1);
		void		uniform3f(int32_t location, float v0, float v1, float v2);
		void		uniform4f(int32_t location, float v0, float v1, float v2, float v3);
		void		uniform1i(int32_t location, int32_t v0);
		void		uniform2i(int32_t location, int32_t v0, int32_t v1);
		void		uniform3i(int32_t location, int32_t v0, int32_t v1, int32_t v2);
		void		uniform4i(int32_t location, int32_t v0, int32_t v1, int32_t v2, int32_t v3);

		void		uniform1fv(int32_t location, int32_t count, const float *value);
		void		uniform2fv(int32_t location, int32_t count, const float *value);
		void		uniform3fv(int32_t location, int32_t count, const float *value);
		void		uniform4fv(int32_t location, int32_t count, const float *value);
		void		uniform1iv(int32_t location, int32_t count, const int32_t *value);
		void		uniform2iv(int32_t location, int32_t count, const int32_t *value);
		void		uniform3iv(int32_t location, int32_t count, const int32_t *value);
		void		uniform4iv(int32_t location, int32_t count, const int32_t *value);

		void		uniformMatrix2fv(int32_t location, int32_t count, bool transpose, const float *value);
		void		uniformMatrix3fv(int32_t location, int32_t count, bool transpose, const float *value);
		void		uniformMatrix4fv(int32_t location, int32_t count, bool transpose, const float *value);

		//-------------------------------------
		// インスタンス取得
		//-------------------------------------
		static GL& getInstance();

	private:
		GL() { };
	};
}
