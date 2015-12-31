//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include <assert.h>
#include <stdio.h>
#include <JAFMouse.h>
#include <JAFWindow.h>
#include <JTL.h>
#include <MOEGUL/EGL.h>
#include <MOEGUL/GL.h>
#include <MOEGUL/GLUtil.h>
#include <MOEGUL/Primitives.h>
#include <MOES/Model.h>

#ifdef WIN32
#include "win/PlatformExtensions.h"
#endif

static void openFile(MOES::ModelIF *model, const char* path)
{
	if (!path) {
		return;
	}
	// Šg’£Žq”»•Ê
	int len = strlen(path);
	if (len < 5) {
		return;
	}
	const char* ext = path + (len - 4);

	if (strcmp(ext, ".mom") == 0) {
		FILE* fp = fopen(path, "rb");
		if (fp) {
			model->loadModelData(fp);
			fclose(fp);
		}
	}
	else if (strcmp(ext, ".moa") == 0) {
		FILE* fp = fopen(path, "rb");
		if (fp) {
			model->loadMotionData(fp);
			fclose(fp);
		}
	}
#ifdef WIN32
	else if (strcmp(ext, ".pmx") == 0) {
		convertAndLoadModel(model, path);
	}
	else if (strcmp(ext, ".vmd") == 0) {
		convertAndLoadMotion(model, path);
	}
#endif
}

int JAFAppMain(int argc, char* argv[])
{
	// MainWindow‚ðŽæ“¾
	JAF::Window* window = JAF::Window::getMainWindow();
	assert(window);

	// EGL Context ‚ð¶¬
	MOEGUL::EGLContextPtr egl = MOEGUL::createNativeEGLContext();
	if (!egl) {
		return 0;
	}
	egl->makeCurrent();

	MOEGUL::GL gl = MOEGUL::GL::getInstance();

	MOEGUL::Camera camera;

	MOES::ModelPtr model = MOES::createModel();

	window->registerFileOpenCallback([&](const char* path) {
		openFile(model.get(), path);
	}
	);
	
	float frame = 0.0f;
	while (window->update()) {
		JAF::MouseState mouse = JAF::getMouseState();

		gl.clearColor(0.0f, 0.5f, 0.0f, 1.0f);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		const float lineColor[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
		const int stageHW = 100;
		for (int x = -stageHW; x <= stageHW; x += 10) {
			MOEGUL::Primitives::drawLine(camera, glm::vec3(x, 0.0f, -stageHW), glm::vec3(x, 0.0f, stageHW), lineColor);
			MOEGUL::Primitives::drawLine(camera, glm::vec3(-stageHW, 0.0f, x), glm::vec3(stageHW, 0.0f, x), lineColor);
		}

		frame += 0.5f;
		if (model) {
			if (frame > model->getMaxFrame()) {
				frame = 0.0f;
			}
			model->setFrame(frame);
			model->update();
			model->draw(&camera);
			//model->drawTest(&camera);
		}

		egl->swapBuffers();
	}

	window->registerFileOpenCallback(JAF::Window::FileOpenCallback());

	return 0;
}
