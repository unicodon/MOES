//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include "JAFPlatform.h"

/// JAF App のエントリポイント。アプリケーション側でこの関数を実装する。
extern "C" int JAFAppMain(int argc, char* argv[]);

namespace JAF
{
	class Window
	{
	public:
		/// フレームワークが用意するメインウィンドウを取得する。
		static Window* getMainWindow();

		//-------------------------------------
		// JAF Window I/F
		// PlatformWindowは以下のI/Fを実装する
		//-------------------------------------
		virtual PlatformDisplay platformDisplay() const = 0;
		virtual bool update() = 0;
		//virtual bool showErrorDialog(const char* msg) = 0;

	protected:
		Window();
		virtual ~Window();
	};
}
