//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "GL.h"

namespace MOEGUL
{
	namespace PresetShaders
	{
		enum ProgramType {
			// Color‚Ì‚Ý
			// attribute vec4 a_position
			// uniform mat4 u_mvp
			// uniform vec4 u_color
			kProgramColor,
			// Color + Texture
			// attribute vec4 a_position
			// attribute vec2 a_uv
			// uniform mat4 u_mvp
			// uniform vec4 u_color
			// uniform Sampler2D u_texture
			kProgramColorTexture,
			kProgramNum
		};

		Program getPresetShader(ProgramType);
	}
}
