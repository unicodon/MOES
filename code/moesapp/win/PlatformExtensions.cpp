//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include "PlatformExtensions.h"

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <JTL.h>

void convertAndLoadModel(MOES::ModelIF *model, const char* path)
{
#if 1
	// コマンドライン準備
	std::string output = path;
	int len = output.length();
	if (len < 5) {
		return;
	}
	output[len - 3] = 'm';
	output[len - 2] = 'o';
	output[len - 1] = 'm';

	std::string commandLine = "pmx2mom.exe";
	commandLine += " -o ";
	commandLine += "\"";
	commandLine += output;
	commandLine += "\" ";
	commandLine += "\"";
	commandLine += path;
	commandLine += "\"";
	std::vector<char> commandBuf;
	commandBuf.resize(commandLine.length() + 1);
	memcpy(&commandBuf[0], commandLine.c_str(), commandLine.length() + 1);

	// 子プロセス生成
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	if (::CreateProcessA(NULL, &commandBuf[0], NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, ".", &si, &pi)) {
		::WaitForSingleObject(pi.hProcess, INFINITE);
		::CloseHandle(pi.hProcess);
		// モデルデータ読み込み
		FILE* fin = fopen(output.c_str(), "rb");
		if (fin) {
			model->loadModelData(fin);
			fclose(fin);
		}
	}
#else
	// 子プロセス向けのパイプを生成。
	// 子プロセスのstdout に書き込み側を渡し、親プロセス側でReadする
	HANDLE hReadPipe;
	HANDLE hWritePipe;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof sa;
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (!::CreatePipe(&hReadPipe, &hWritePipe, &sa, 1024 * 1024)) {
		return;
	}

	// hReadPipeを元にFILEを作る
	int fd = _open_osfhandle((intptr_t)hReadPipe, _O_RDONLY);
	if (fd < 0) {
		::CloseHandle(hReadPipe);
		::CloseHandle(hWritePipe);
		return;
	}
	FILE* fin = _fdopen(fd, "rb");
	if (!fin) {
		_close(fd); // hReadPipeはfdを閉じるとCloseされる
		::CloseHandle(hWritePipe);
	}

	// 子プロセス生成
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = hWritePipe;
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	// コマンドライン準備
	std::string commandLine = "pmx2mom.exe";
	commandLine += " --stdout ";
	commandLine += "\"";
	commandLine += path;
	commandLine += "\"";
	std::vector<char> commandBuf;
	commandBuf.resize(commandLine.length() + 1);
	memcpy(&commandBuf[0], commandLine.c_str(), commandLine.length() + 1);

	if (::CreateProcessA(NULL, &commandBuf[0], &sa, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, ".", &si, &pi)) {
		// モデルデータ読み込み
		model->loadModelData(fin);

		::CloseHandle(pi.hProcess);
	}

	fclose(fin);
	::CloseHandle(hWritePipe);
#endif
}

void convertAndLoadMotion(MOES::ModelIF *model, const char* path)
{
	// コマンドライン準備
	std::string output = path;
	int len = output.length();
	if (len < 5) {
		return;
	}
	output[len - 3] = 'm';
	output[len - 2] = 'o';
	output[len - 1] = 'a';

	std::string commandLine = "vmd2moa.exe";
	commandLine += " -o ";
	commandLine += "\"";
	commandLine += output;
	commandLine += "\" ";
	commandLine += "\"";
	commandLine += path;
	commandLine += "\"";
	std::vector<char> commandBuf;
	commandBuf.resize(commandLine.length() + 1);
	memcpy(&commandBuf[0], commandLine.c_str(), commandLine.length() + 1);

	// 子プロセス生成
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	if (::CreateProcessA(NULL, &commandBuf[0], NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, ".", &si, &pi)) {
		::WaitForSingleObject(pi.hProcess, INFINITE);
		::CloseHandle(pi.hProcess);
		// モーションデータ読み込み
		FILE* fin = fopen(output.c_str(), "rb");
		if (fin) {
			model->loadMotionData(fin);
			fclose(fin);
		}
	}
}
