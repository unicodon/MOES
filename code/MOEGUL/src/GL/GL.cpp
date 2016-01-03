//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include <MOEGUL/GL.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "debug.h"

namespace
{
	template <typename PFNGLPROC>
	struct APIEntry {
		APIEntry(const char* name)
		{
			m_proc = reinterpret_cast<PFNGLPROC>(::eglGetProcAddress(name));
			if (!m_proc) {
				DBG_ERROR("WARNING: %s is not available.\n", name);
			}
			else {
				DBG_INFO("GLAPI %s bound.\n", name);
			}
		}
		PFNGLPROC m_proc;
	};

	struct APITable {
		APIEntry<PFNGLATTACHSHADERPROC> m_attachShader;
		APIEntry<PFNGLBINDBUFFERPROC> m_bindBuffer;
		APIEntry<PFNGLBINDTEXTUREPROC> m_bindTexture;
		APIEntry<PFNGLBLENDFUNCPROC> m_blendFunc;
		APIEntry<PFNGLBUFFERDATAPROC> m_bufferData;
		APIEntry<PFNGLCLEARPROC> m_clear;
		APIEntry<PFNGLCLEARCOLORPROC> m_clearColor;
		APIEntry<PFNGLCREATEPROGRAMPROC> m_createProgram;
		APIEntry<PFNGLCREATESHADERPROC> m_createShader;
		APIEntry<PFNGLCOMPILESHADERPROC> m_compileShader;
		APIEntry<PFNGLCOMPRESSEDTEXIMAGE2DPROC> m_compressedTexImage2D;
		APIEntry<PFNGLCULLFACEPROC> m_cullFace;
		APIEntry<PFNGLDELETEBUFFERSPROC> m_deleteBuffers;
		APIEntry<PFNGLDELETEPROGRAMPROC> m_deleteProgram;
		APIEntry<PFNGLDELETESHADERPROC> m_deleteShader;
		APIEntry<PFNGLDELETETEXTURESPROC> m_deleteTextures;
		APIEntry<PFNGLDISABLEPROC> m_disable;
		APIEntry<PFNGLDISABLEVERTEXATTRIBARRAYPROC> m_disableVertexAttribArray;
		APIEntry<PFNGLDRAWARRAYSPROC> m_drawArrays;
		APIEntry<PFNGLDRAWELEMENTSPROC> m_drawElements;
		APIEntry<PFNGLENABLEPROC> m_enable;
		APIEntry<PFNGLENABLEVERTEXATTRIBARRAYPROC> m_enableVertexAttribArray;
		APIEntry<PFNGLFRONTFACEPROC> m_frontFace;
		APIEntry<PFNGLGENBUFFERSPROC> m_genBuffers;
		APIEntry<PFNGLGENTEXTURESPROC> m_genTextures;
		APIEntry<PFNGLGETATTRIBLOCATIONPROC> m_getAttribLocation;
		APIEntry<PFNGLGETERRORPROC> m_getError;
		APIEntry<PFNGLGETPROGRAMINFOLOGPROC> m_getProgramInfoLog;
		APIEntry<PFNGLGETPROGRAMIVPROC> m_getProgramiv;
		APIEntry<PFNGLGETSHADERINFOLOGPROC> m_getShaderInfoLog;
		APIEntry<PFNGLGETSHADERIVPROC> m_getShaderiv;
		APIEntry<PFNGLGETUNIFORMLOCATIONPROC> m_getUniformLocation;
		APIEntry<PFNGLLINKPROGRAMPROC> m_linkProgram;
		APIEntry<PFNGLSHADERSOURCEPROC> m_shaderSource;
		APIEntry<PFNGLTEXIMAGE2DPROC> m_texImage2D;
		APIEntry<PFNGLTEXPARAMETERFPROC> m_texParameterf;
		APIEntry<PFNGLTEXPARAMETERFVPROC> m_texParameterfv;
		APIEntry<PFNGLTEXPARAMETERIPROC> m_texParameteri;
		APIEntry<PFNGLTEXPARAMETERIVPROC> m_texParameteriv;
		APIEntry<PFNGLUNIFORM1FPROC> m_uniform1f;
		APIEntry<PFNGLUNIFORM1FVPROC> m_uniform1fv;
		APIEntry<PFNGLUNIFORM1IPROC> m_uniform1i;
		APIEntry<PFNGLUNIFORM1IVPROC> m_uniform1iv;
		APIEntry<PFNGLUNIFORM2FPROC> m_uniform2f;
		APIEntry<PFNGLUNIFORM2FVPROC> m_uniform2fv;
		APIEntry<PFNGLUNIFORM2IPROC> m_uniform2i;
		APIEntry<PFNGLUNIFORM2IVPROC> m_uniform2iv;
		APIEntry<PFNGLUNIFORM3FPROC> m_uniform3f;
		APIEntry<PFNGLUNIFORM3FVPROC> m_uniform3fv;
		APIEntry<PFNGLUNIFORM3IPROC> m_uniform3i;
		APIEntry<PFNGLUNIFORM3IVPROC> m_uniform3iv;
		APIEntry<PFNGLUNIFORM4FPROC> m_uniform4f;
		APIEntry<PFNGLUNIFORM4FVPROC> m_uniform4fv;
		APIEntry<PFNGLUNIFORM4IPROC> m_uniform4i;
		APIEntry<PFNGLUNIFORM4IVPROC> m_uniform4iv;
		APIEntry<PFNGLUNIFORMMATRIX2FVPROC> m_uniformMatrix2fv;
		APIEntry<PFNGLUNIFORMMATRIX3FVPROC> m_uniformMatrix3fv;
		APIEntry<PFNGLUNIFORMMATRIX4FVPROC> m_uniformMatrix4fv;
		APIEntry<PFNGLUSEPROGRAMPROC> m_useProgram;
		APIEntry<PFNGLVERTEXATTRIBPOINTERPROC> m_vertexAttribPointer;
		APIEntry<PFNGLVIEWPORTPROC> m_viewport;

		APITable()
			: m_attachShader("glAttachShader")
			, m_bindBuffer("glBindBuffer")
			, m_bindTexture("glBindTexture")
			, m_blendFunc("glBlendFunc")
			, m_bufferData("glBufferData")
			, m_clear("glClear")
			, m_clearColor("glClearColor")
			, m_createProgram("glCreateProgram")
			, m_createShader("glCreateShader")
			, m_compileShader("glCompileShader")
			, m_compressedTexImage2D("glCompressedTexImage2D")
			, m_cullFace("glCullFace")
			, m_deleteBuffers("glDeleteBuffers")
			, m_deleteProgram("glDeleteProgram")
			, m_deleteShader("glDeleteShader")
			, m_deleteTextures("glDeleteTextures")
			, m_disable("glDisable")
			, m_disableVertexAttribArray("glDisableVertexAttribArray")
			, m_drawArrays("glDrawArrays")
			, m_drawElements("glDrawElements")
			, m_enable("glEnable")
			, m_enableVertexAttribArray("glEnableVertexAttribArray")
			, m_frontFace("glFrontFace")
			, m_genBuffers("glGenBuffers")
			, m_genTextures("glGenTextures")
			, m_getAttribLocation("glGetAttribLocation")
			, m_getError("glGetError")
			, m_getProgramInfoLog("glGetProgramInfoLog")
			, m_getProgramiv("glGetProgramiv")
			, m_getShaderInfoLog("glGetShaderInfoLog")
			, m_getShaderiv("glGetShaderiv")
			, m_getUniformLocation("glGetUniformLocation")
			, m_linkProgram("glLinkProgram")
			, m_shaderSource("glShaderSource")
			, m_texImage2D("glTexImage2D")
			, m_texParameterf("glTexParameterf")
			, m_texParameterfv("glTexParameterfv")
			, m_texParameteri("glTexParameteri")
			, m_texParameteriv("glTexParameteriv")
			, m_uniform1f("glUniform1f")
			, m_uniform1fv("glUniform1fv")
			, m_uniform1i("glUniform1i")
			, m_uniform1iv("glUniform1iv")
			, m_uniform2f("glUniform2f")
			, m_uniform2fv("glUniform2fv")
			, m_uniform2i("glUniform2i")
			, m_uniform2iv("glUniform2iv")
			, m_uniform3f("glUniform3f")
			, m_uniform3fv("glUniform3fv")
			, m_uniform3i("glUniform3i")
			, m_uniform3iv("glUniform3iv")
			, m_uniform4f("glUniform4f")
			, m_uniform4fv("glUniform4fv")
			, m_uniform4i("glUniform4i")
			, m_uniform4iv("glUniform4iv")
			, m_uniformMatrix2fv("glUniformMatrix2fv")
			, m_uniformMatrix3fv("glUniformMatrix3fv")
			, m_uniformMatrix4fv("glUniformMatrix4fv")
			, m_useProgram("glUseProgram")
			, m_vertexAttribPointer("glVertexAttribPointer")
			, m_viewport("glViewport")
		{
		}
	};

	APITable& api() {
		// このタイミングでAPIテーブルを初期化
		static APITable s_api;
		return s_api;
	}

#define HAS_API(name) static_cast<bool>(api().m_##name.m_proc)
#define CALL_API(name, ...) (api().m_##name.m_proc)(__VA_ARGS__)
#define SHOW_GL_ERROR() do { GLenum e = getError(); if (e != GL_NO_ERROR) { DBG_ERROR("GLError %X\n", e); } } while (0)

	GLuint getId(MOEGUL::GLObj* obj) { return obj ? (GLuint)obj->id() : 0; }
}

namespace MOEGUL
{
	//-------------------------------------
	// API (in alphabetical order)
	//-------------------------------------
	void GL::attachShader(Program program, Shader shader)
	{
		if (HAS_API(attachShader)) {
			CALL_API(attachShader, getId(program.get()), getId(shader.get()));
			SHOW_GL_ERROR();
		}
	}

	void GL::bindBuffer(uint32_t target, Buffer buffer)
	{
		if (HAS_API(bindBuffer)) {
			CALL_API(bindBuffer, target, getId(buffer.get()));
			SHOW_GL_ERROR();
		}
	}

	void GL::bindTexture(uint32_t target, Texture texture)
	{
		if (HAS_API(bindTexture)) {
			CALL_API(bindTexture, target, getId(texture.get()));
			SHOW_GL_ERROR();
		}
	}

	void GL::blendFunc(uint32_t sfactor, uint32_t dfactor)
	{
		if (HAS_API(blendFunc)) {
			CALL_API(blendFunc, sfactor, dfactor);
			SHOW_GL_ERROR();
		}
	}

	void GL::bufferData(uint32_t target, uint32_t size, const void* data, uint32_t usage)
	{
		if (HAS_API(bufferData)) {
			CALL_API(bufferData, target, size, data, usage);
			SHOW_GL_ERROR();
		}
	}

	void GL::clear(uint32_t mask)
	{
		if (HAS_API(clear)) {
			CALL_API(clear, mask);
			SHOW_GL_ERROR();
		}
	}

	void GL::clearColor(float red, float green, float blue, float alpha)
	{
		if (HAS_API(clearColor)) {
			CALL_API(clearColor, red, green, blue, alpha);
			SHOW_GL_ERROR();
		}
	}

	void GL::compileShader(Shader shader)
	{
		if (HAS_API(compileShader)) {
			CALL_API(compileShader, getId(shader.get()));
			SHOW_GL_ERROR();
		}
	}

	void GL::compressedTexImage2D(uint32_t target, int32_t level, uint32_t internalformat, int32_t width, int32_t height, int32_t border, int32_t imageSize, const void *data)
	{
		if (HAS_API(compressedTexImage2D)) {
			CALL_API(compressedTexImage2D, target, level, internalformat, width, height, border, imageSize, data);
			SHOW_GL_ERROR();
		}
	}

	Buffer GL::createBuffer()
	{
		if (HAS_API(genBuffers)) {
			GLuint id;
			CALL_API(genBuffers, 1, &id);
			SHOW_GL_ERROR();
			if (getError() == 0) {
				return BufferObj::create(id);
			}
		}
		return Buffer();
	}

	Program GL::createProgram()
	{
		if (HAS_API(createProgram)) {
			GLuint id = CALL_API(createProgram);
			SHOW_GL_ERROR();
			if (id != 0) {
				return ProgramObj::create(id);
			}
		}
		return Program();
	}

	Shader GL::createShader(uint32_t type)
	{
		if (HAS_API(createShader)) {
			GLuint id = CALL_API(createShader, type);
			SHOW_GL_ERROR();
			if (id != 0) {
				return ShaderObj::create(id);
			}
		}
		return Shader();
	}

	Texture GL::createTexture()
	{
		if (HAS_API(genTextures)) {
			GLuint id;
			CALL_API(genTextures, 1, &id);
			SHOW_GL_ERROR();
			if (getError() == 0) {
				return TextureObj::create(id);
			}
		}
		return Texture();
	}

	void GL::cullFace(uint32_t mode)
	{
		if (HAS_API(cullFace)) {
			CALL_API(cullFace, mode);
			SHOW_GL_ERROR();
		}
	}

	void GL::disable(uint32_t cap)
	{
		if (HAS_API(disable)) {
			CALL_API(disable, cap);
			SHOW_GL_ERROR();
		}
	}

	void GL::disableVertexAttribArray(uint32_t idx)
	{
		if (HAS_API(disableVertexAttribArray)) {
			CALL_API(disableVertexAttribArray, idx);
			SHOW_GL_ERROR();
		}
	}

	void GL::drawArrays(uint32_t mode, int32_t first, int32_t count)
	{
		if (HAS_API(drawArrays)) {
			CALL_API(drawArrays, mode, first, count);
			SHOW_GL_ERROR();
		}
	}

	void GL::drawElements(uint32_t mode, int32_t count, uint32_t type, void* offset)
	{
		if (HAS_API(drawElements)) {
			CALL_API(drawElements, mode, count, type, offset);
			SHOW_GL_ERROR();
		}
	}

	void GL::enable(uint32_t cap)
	{
		if (HAS_API(enable)) {
			CALL_API(enable, cap);
			SHOW_GL_ERROR();
		}
	}

	void GL::enableVertexAttribArray(uint32_t idx)
	{
		if (HAS_API(enableVertexAttribArray)) {
			CALL_API(enableVertexAttribArray, idx);
			SHOW_GL_ERROR();
		}
	}

	void GL::frontFace(uint32_t mode)
	{
		if (HAS_API(frontFace)) {
			CALL_API(frontFace, mode);
			SHOW_GL_ERROR();
		}
	}

	int32_t	GL::getAttribLocation(Program program, const char* name)
	{
		if (HAS_API(getAttribLocation)) {
			return CALL_API(getAttribLocation, getId(program.get()), name);
		}
		return -1;
	}

	uint32_t GL::getError()
	{
		if (HAS_API(getError)) {
			return CALL_API(getError);
		}
		return INVALID_OPERATION;
	}

	std::string GL::getProgramInfoLog(Program program)
	{
		if (!HAS_API(getProgramInfoLog)) {
			return std::string();
		}
		int32_t logLen = getProgramParameter(program, INFO_LOG_LENGTH);
		if (logLen > 0) {
			std::string log;
			log.resize(logLen);
			CALL_API(getProgramInfoLog, getId(program.get()), logLen, &logLen, &log[0]);
			return log;
		}
		return std::string();
	}

	int32_t GL::getProgramParameter(Program program, uint32_t pname)
	{
		if (!HAS_API(getProgramiv)) {
			return 0;
		}
		GLint params;
		CALL_API(getProgramiv, getId(program.get()), pname, &params);
		if (getError() == GL_NO_ERROR) {
			return params;
		}
		return 0;
	}

	std::string GL::getShaderInfoLog(Shader shader)
	{
		if (!HAS_API(getShaderInfoLog)) {
			return std::string();
		}
		int32_t logLen = getShaderParameter(shader, INFO_LOG_LENGTH);
		if (logLen > 0) {
			std::string log;
			log.resize(logLen);
			CALL_API(getShaderInfoLog, getId(shader.get()), logLen, &logLen, &log[0]);
			return log;
		}
		return std::string();
	}

	int32_t GL::getShaderParameter(Shader shader, uint32_t pname)
	{
		if (!HAS_API(getShaderiv)) {
			return 0;
		}
		GLint params;
		CALL_API(getShaderiv, getId(shader.get()), pname, &params);
		if (getError() == GL_NO_ERROR) {
			return params;
		}
		return 0;
	}

	int32_t	GL::getUniformLocation(Program program, const char* name)
	{
		if (HAS_API(getUniformLocation)) {
			return CALL_API(getUniformLocation, getId(program.get()), name);
		}
		return -1;
	}

	void GL::linkProgram(Program program)
	{
		if (HAS_API(linkProgram)) {
			CALL_API(linkProgram, getId(program.get()));
			SHOW_GL_ERROR();
		}
	}

	void GL::shaderSource(Shader shader, const char* source)
	{
		if (HAS_API(shaderSource)) {
			CALL_API(shaderSource, getId(shader.get()), 1, &source, NULL);
			SHOW_GL_ERROR();
		}
	}

	void GL::texImage2D(uint32_t target, int32_t level, uint32_t internalformat, int32_t width, int32_t height, int32_t border, uint32_t format, int32_t type, const void *pixels)
	{
		if (HAS_API(texImage2D)) {
			CALL_API(texImage2D, target, level, internalformat, width, height, border, format, type, pixels);
			SHOW_GL_ERROR();
		}
	}

	void GL::texParameterf(uint32_t target, uint32_t pname, float param)
	{
		if (HAS_API(texParameterf)) {
			CALL_API(texParameterf, target, pname, param);
			SHOW_GL_ERROR();
		}
	}

	void GL::texParameterfv(uint32_t target, uint32_t pname, const float* params)
	{
		if (HAS_API(texParameterfv)) {
			CALL_API(texParameterfv, target, pname, params);
			SHOW_GL_ERROR();
		}
	}

	void GL::texParameteri(uint32_t target, uint32_t pname, int32_t param)
	{
		if (HAS_API(texParameteri)) {
			CALL_API(texParameteri, target, pname, param);
			SHOW_GL_ERROR();
		}
	}

	void GL::texParameteriv(uint32_t target, uint32_t pname, const int32_t* params)
	{
		if (HAS_API(texParameteriv)) {
			CALL_API(texParameteriv, target, pname, params);
			SHOW_GL_ERROR();
		}
	}

	void GL::useProgram(Program program)
	{
		if (HAS_API(useProgram)) {
			CALL_API(useProgram, getId(program.get()));
			SHOW_GL_ERROR();
		}
	}

	void GL::vertexAttribPointer(uint32_t idx, int32_t size, uint32_t type, bool normalized, int32_t stride, void* offset)
	{
		if (HAS_API(vertexAttribPointer)) {
			CALL_API(vertexAttribPointer, idx, size, type, normalized, stride, offset);
			SHOW_GL_ERROR();
		}
	}

	void GL::viewport(int32_t x, int32_t y, int32_t width, int32_t height)
	{
		if (HAS_API(viewport)) {
			CALL_API(viewport, x, y, width, height);
			SHOW_GL_ERROR();
		}
	}

	//-------------------------------------
	// GL API (uniform)
	//-------------------------------------
	void GL::uniform1f(int32_t location, float v0)
	{
		if (HAS_API(uniform1f)) {
			CALL_API(uniform1f, location, v0);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform2f(int32_t location, float v0, float v1)
	{
		if (HAS_API(uniform2f)) {
			CALL_API(uniform2f, location, v0, v1);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform3f(int32_t location, float v0, float v1, float v2)
	{
		if (HAS_API(uniform3f)) {
			CALL_API(uniform3f, location, v0, v1, v2);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform4f(int32_t location, float v0, float v1, float v2, float v3)
	{
		if (HAS_API(uniform4f)) {
			CALL_API(uniform4f, location, v0, v1, v2, v3);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform1i(int32_t location, int32_t v0)
	{
		if (HAS_API(uniform1i)) {
			CALL_API(uniform1i, location, v0);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform2i(int32_t location, int32_t v0, int32_t v1)
	{
		if (HAS_API(uniform2i)) {
			CALL_API(uniform2i, location, v0, v1);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform3i(int32_t location, int32_t v0, int32_t v1, int32_t v2)
	{
		if (HAS_API(uniform3i)) {
			CALL_API(uniform3i, location, v0, v1, v2);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform4i(int32_t location, int32_t v0, int32_t v1, int32_t v2, int32_t v3)
	{
		if (HAS_API(uniform4i)) {
			CALL_API(uniform4i, location, v0, v1, v2, v3);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform1fv(int32_t location, int32_t count, const float *value)
	{
		if (HAS_API(uniform1fv)) {
			CALL_API(uniform1fv, location, count, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform2fv(int32_t location, int32_t count, const float *value)
	{
		if (HAS_API(uniform2fv)) {
			CALL_API(uniform2fv, location, count, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform3fv(int32_t location, int32_t count, const float *value)
	{
		if (HAS_API(uniform3fv)) {
			CALL_API(uniform3fv, location, count, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform4fv(int32_t location, int32_t count, const float *value)
	{
		if (HAS_API(uniform4fv)) {
			CALL_API(uniform4fv, location, count, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform1iv(int32_t location, int32_t count, const int32_t *value)
	{
		if (HAS_API(uniform1iv)) {
			CALL_API(uniform1iv, location, count, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform2iv(int32_t location, int32_t count, const int32_t *value)
	{
		if (HAS_API(uniform2iv)) {
			CALL_API(uniform2iv, location, count, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform3iv(int32_t location, int32_t count, const int32_t *value)
	{
		if (HAS_API(uniform3iv)) {
			CALL_API(uniform3iv, location, count, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniform4iv(int32_t location, int32_t count, const int32_t *value)
	{
		if (HAS_API(uniform4iv)) {
			CALL_API(uniform4iv, location, count, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniformMatrix2fv(int32_t location, int32_t count, bool transpose, const float *value)
	{
		if (HAS_API(uniformMatrix2fv)) {
			CALL_API(uniformMatrix2fv, location, count, transpose, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniformMatrix3fv(int32_t location, int32_t count, bool transpose, const float *value)
	{
		if (HAS_API(uniformMatrix3fv)) {
			CALL_API(uniformMatrix3fv, location, count, transpose, value);
			SHOW_GL_ERROR();
		}
	}

	void GL::uniformMatrix4fv(int32_t location, int32_t count, bool transpose, const float *value)
	{
		if (HAS_API(uniformMatrix4fv)) {
			CALL_API(uniformMatrix4fv, location, count, transpose, value);
			SHOW_GL_ERROR();
		}
	}

	//-------------------------------------
	// Get instance
	//-------------------------------------
	GL& GL::getInstance()
	{
		static GL s_gl;
		return s_gl;
	}

	//-------------------------------------
	// GL object dtor
	//-------------------------------------
	BufferObj::~BufferObj()
	{
		if (HAS_API(deleteBuffers)) {
			CALL_API(deleteBuffers, 1, &m_id);
		}
	}

	ProgramObj::~ProgramObj()
	{
		if (HAS_API(deleteProgram)) {
			CALL_API(deleteProgram, m_id);
		}
	}

	ShaderObj::~ShaderObj()
	{
		if (HAS_API(deleteShader)) {
			CALL_API(deleteShader, m_id);
		}
	}

	TextureObj::~TextureObj()
	{
		if (HAS_API(deleteTextures)) {
			CALL_API(deleteTextures, 1, &m_id);
		}
	}
}
