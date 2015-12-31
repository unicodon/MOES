//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include "JAFPlatform.h"

#include <functional>

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
		virtual PlatformWindowHandle platformWindowHandle() const = 0;
		virtual PlatformDisplay platformDisplay() const = 0;
		virtual bool update() = 0;
		//virtual bool showErrorDialog(const char* msg) = 0;

		/// Window に対してファイルOpen要求があった場合に、ファイルパスを
		/// UTF-8 文字列で通知するためのコールバック
		typedef std::function< void(const char*)> FileOpenCallback;

		virtual void registerFileOpenCallback(const FileOpenCallback&) = 0;

	protected:
		Window();
		virtual ~Window();
	};
}
