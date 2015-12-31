//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <glm/glm.hpp>
#include <MOEGUL/Camera.h>
#include "ModelData/ModelData.h"

namespace MOES
{
	/// ƒ‚ƒfƒ‹‚ð•`‰æ‚·‚é
	void renderModel(
		const MOEGUL::Camera* camera,
		const ModelData* model,
		const std::vector<glm::mat4>& boneTransforms,
		const std::vector<float>& vertexTransforms
		);
}
