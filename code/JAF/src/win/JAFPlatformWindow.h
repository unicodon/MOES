//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <JAFWindow.h>

#include <windows.h>

namespace JAF
{
	class PlatformWindow :
		public Window
	{
	public:
		PlatformWindow();
		virtual ~PlatformWindow();

		//-------------------------------------
		// JAFWindow API
		//-------------------------------------
		virtual PlatformWindowHandle platformWindowHandle() const { return m_hwnd; };
		virtual PlatformDisplay platformDisplay() const { return m_hdc; }
		virtual bool update();
		virtual bool showErrorDialog(const char* msg);

		//-------------------------------------
		// JAFWindow callback
		//-------------------------------------

		/// Window に対してファイルOpen要求があった場合に、ファイルパスを
		/// UTF-8 文字列で通知するためのコールバック
		typedef std::function< void(const char*)> FileOpenCallback;

		virtual void registerFileOpenCallback(const FileOpenCallback&);

	private:
		static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		void onDropFile(HDROP);

		HWND m_hwnd;
		HDC m_hdc;

		FileOpenCallback m_fileOpenCallback;
	};

}
