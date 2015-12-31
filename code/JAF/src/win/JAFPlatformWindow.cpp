//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "JAFPlatformWindow.h"

#include <stdio.h>
#include <windowsx.h>
#include <JAFMouse.h>
#include <JTL.h>

namespace
{
	const char *kWindowClassName = "JAFFrameWindow";
	JAF::PlatformWindow *s_window = nullptr;
	JAF::MouseState s_mouse;

	void setMousePos(HWND hWnd, LPARAM lParam)
	{
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		float x = GET_X_LPARAM(lParam);
		float y = GET_Y_LPARAM(lParam);
		s_mouse.x = x / (clientRect.right - clientRect.left);
		s_mouse.y = y / (clientRect.bottom - clientRect.top);
	}
}

namespace JAF
{
	// MouseState
	MouseState getMouseState()
	{
		return s_mouse;
	}

	// Window
	PlatformWindow::PlatformWindow()
	{
		WNDCLASSEXA windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEXA);
		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = wndProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = GetModuleHandle(NULL);
		windowClass.hIcon = NULL;
		windowClass.hCursor = NULL;
		windowClass.hbrBackground = 0;
		windowClass.lpszMenuName = NULL;
		windowClass.lpszClassName = kWindowClassName;
		if (!::RegisterClassExA(&windowClass))
		{
			abort();
		}

		DWORD parentStyle = WS_OVERLAPPED | WS_SYSMENU | WS_MAXIMIZEBOX;
		DWORD parentExtendedStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES;

		RECT sizeRect = { 0, 0, static_cast<LONG>(640), static_cast<LONG>(720) };
		::AdjustWindowRectEx(&sizeRect, parentStyle, FALSE, parentExtendedStyle);

		m_hwnd = ::CreateWindowExA(parentExtendedStyle, kWindowClassName, "TBD", parentStyle,
			CW_USEDEFAULT, CW_USEDEFAULT, sizeRect.right - sizeRect.left,
			sizeRect.bottom - sizeRect.top, NULL, NULL, GetModuleHandle(NULL), this);

		m_hdc = ::GetDC(m_hwnd);
	}

	PlatformWindow::~PlatformWindow()
	{
	}

	LRESULT CALLBACK PlatformWindow::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc;

		switch (message)
		{
		case WM_CREATE:
			// Drag & Drop を有効にする
			::DragAcceptFiles(hWnd, TRUE);
			break;
		case WM_CLOSE:
			// Close 要求が来たら問答無用でWindow破棄
			::DestroyWindow(hWnd);
			break;

		case WM_DROPFILES:
			s_window->onDropFile((HDROP)wParam);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_LBUTTONDOWN:
			s_mouse.buttons |= MouseState::kMouseButtonLeft;
			setMousePos(hWnd, lParam);
			::SetCapture(hWnd);
			s_mouse.isValid = true;
			break;

		case WM_LBUTTONUP:
			s_mouse.buttons &= ~MouseState::kMouseButtonLeft;
			if (!s_mouse.buttons) {
				::ReleaseCapture();
				s_mouse.isValid = false;
			}
			break;

		case WM_RBUTTONDOWN:
			s_mouse.buttons |= MouseState::kMouseButtonRight;
			setMousePos(hWnd, lParam);
			::SetCapture(hWnd);
			s_mouse.isValid = true;
			break;

		case WM_RBUTTONUP:
			s_mouse.buttons &= ~MouseState::kMouseButtonRight;
			if (!s_mouse.buttons) {
				::ReleaseCapture();
				s_mouse.isValid = false;
			}
			break;

		case WM_MOUSEMOVE:
			setMousePos(hWnd, lParam);
			break;

		default:
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	// JAFWindow API
	bool PlatformWindow::update()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				// QUITメッセージが来たらfalseを返す
				return false;
			}
		}
		return true;
	}

	bool PlatformWindow::showErrorDialog(const char* msg)
	{
		::MessageBoxA(m_hwnd, msg, "Error", MB_OK);
		return true;
	}

	void PlatformWindow::registerFileOpenCallback(const FileOpenCallback& callback)
	{
		m_fileOpenCallback = callback;
	}

	void PlatformWindow::onDropFile(HDROP hDrop)
	{
		char buf[MAX_PATH];
		if (::DragQueryFileA(hDrop, 0, buf, MAX_PATH)) {
			if (m_fileOpenCallback) {
				m_fileOpenCallback(buf);
			}
		}
		::DragFinish(hDrop);
	}

	//
	// JAFWindow
	//
	Window* Window::getMainWindow()
	{
		return s_window;
	}
}

// Console initializer/finalizer
namespace {
	class Console {
	public:
		Console()
		{
			UINT cp = CP_UTF8;
			::AllocConsole();
			freopen("CONOUT$", "w", stdout);
			freopen("CONIN$", "r", stdin);
			::SetConsoleCP(cp);
			::SetConsoleOutputCP(cp);

			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD nFont = 12;
			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = nFont;
			cfi.dwFontSize = ::GetConsoleFontSize(hConsole, nFont);
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = 400;
			lstrcpy(cfi.FaceName, TEXT("MS Gothic"));
			::SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);
		}

		~Console()
		{
			::FreeConsole();
		}
	};
}

//
// WinMain
//
int WINAPI WinMain(
	HINSTANCE hInstance,      // 現在のインスタンスのハンドル
	HINSTANCE hPrevInstance,  // 以前のインスタンスのハンドル
	LPSTR lpCmdLine,          // コマンドライン
	int nCmdShow              // 表示状態
	)
{
#ifdef _DEBUG
	// デバッグビルドではコンソールを有効化
	Console console;
#endif

	// MainWindow生成
	JAF::PlatformWindow window;
	s_window = &window;

	// マウス状態初期化
	memset(&s_mouse, 0, sizeof(s_mouse));

	// Windowを指定の表示状態にする
	::ShowWindow(window.platformWindowHandle(), nCmdShow);

	// AppMain に制御を移す
	int res = JAFAppMain(0, 0);

	// Window 破棄
	s_window = nullptr;

	return 0;
}
