//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <MOEGUL/PresetShaders.h>

#include <JTL.h>
#include <MOEGUL/EGL.h>
#include <MOEGUL/GLUtil.h>

namespace
{
	MOEGUL::Program colorProgram()
	{
		static MOEGUL::Program s_prog;
		if (s_prog) {
			return s_prog;
		}

		const char *vs = STRINGIFY(
			//---VERTEX SHADER---
			attribute vec4 a_position;
			uniform mat4 u_mvp;
			void main()
			{
				gl_Position = u_mvp * a_position;
			}
			//---VERTEX SHADER---
		);
		const char *fs = STRINGIFY(
			//---FRAGMENT SHADER---
			precision mediump float;
			uniform vec4 u_color;
			void main()
			{
				gl_FragColor = u_color;
			}
			//---FRAGMENT SHADER---
		);

		s_prog = MOEGUL::buildShaderProgram(vs, fs);

		// EGLContext”jŠüŽž‚ÉProgram‚à”jŠü
		MOEGUL::EGL::registerCleanupFunction([]() {
			s_prog.reset();
		});

		return s_prog;
	}

	MOEGUL::Program colorTextureProgram()
	{
		static MOEGUL::Program s_prog;
		if (s_prog) {
			return s_prog;
		}

		const char *vs = STRINGIFY(
			//---VERTEX SHADER---
			attribute vec4 a_position;
			attribute vec2 a_uv; 
			varying vec2 v_uv;
			uniform mat4 u_mvp;
			void main()
			{
				v_uv = a_uv;
				gl_Position = u_mvp * a_position;
			}
			//---VERTEX SHADER---
		);
		const char *fs = STRINGIFY(
			//---FRAGMENT SHADER---
			precision mediump float;
			varying vec2 v_uv;
			uniform vec4 u_color;
			uniform sampler2D u_texture;
			void main()
			{
				gl_FragColor = texture2D(u_texture, v_uv) * u_color;
			}
			//---FRAGMENT SHADER---
		);

		s_prog = MOEGUL::buildShaderProgram(vs, fs);

		// EGLContext”jŠüŽž‚ÉProgram‚à”jŠü
		MOEGUL::EGL::registerCleanupFunction([]() {
			s_prog.reset();
		});

		return s_prog;
	}
}

namespace MOEGUL
{
	namespace PresetShaders
	{
		Program getPresetShader(ProgramType type)
		{
			switch (type) {
			case kProgramColor:
				return colorProgram();
			case kProgramColorTexture:
				return colorTextureProgram();
			default:
				return Program();
			}
		}
	}
}
