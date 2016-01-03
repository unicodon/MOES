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
	// �R�}���h���C������
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

	// �q�v���Z�X����
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	if (::CreateProcessA(NULL, &commandBuf[0], NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, ".", &si, &pi)) {
		::WaitForSingleObject(pi.hProcess, INFINITE);
		::CloseHandle(pi.hProcess);
		// ���f���f�[�^�ǂݍ���
		FILE* fin = fopen(output.c_str(), "rb");
		if (fin) {
			model->loadModelData(fin);
			fclose(fin);
		}
	}
#else
	// �q�v���Z�X�����̃p�C�v�𐶐��B
	// �q�v���Z�X��stdout �ɏ������ݑ���n���A�e�v���Z�X����Read����
	HANDLE hReadPipe;
	HANDLE hWritePipe;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof sa;
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (!::CreatePipe(&hReadPipe, &hWritePipe, &sa, 1024 * 1024)) {
		return;
	}

	// hReadPipe������FILE�����
	int fd = _open_osfhandle((intptr_t)hReadPipe, _O_RDONLY);
	if (fd < 0) {
		::CloseHandle(hReadPipe);
		::CloseHandle(hWritePipe);
		return;
	}
	FILE* fin = _fdopen(fd, "rb");
	if (!fin) {
		_close(fd); // hReadPipe��fd������Close�����
		::CloseHandle(hWritePipe);
	}

	// �q�v���Z�X����
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = hWritePipe;
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	// �R�}���h���C������
	std::string commandLine = "pmx2mom.exe";
	commandLine += " --stdout ";
	commandLine += "\"";
	commandLine += path;
	commandLine += "\"";
	std::vector<char> commandBuf;
	commandBuf.resize(commandLine.length() + 1);
	memcpy(&commandBuf[0], commandLine.c_str(), commandLine.length() + 1);

	if (::CreateProcessA(NULL, &commandBuf[0], &sa, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, ".", &si, &pi)) {
		// ���f���f�[�^�ǂݍ���
		model->loadModelData(fin);

		::CloseHandle(pi.hProcess);
	}

	fclose(fin);
	::CloseHandle(hWritePipe);
#endif
}

void convertAndLoadMotion(MOES::ModelIF *model, const char* path)
{
	// �R�}���h���C������
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

	// �q�v���Z�X����
	PROCESS_INFORMATION pi;
	STARTUPINFOA si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	if (::CreateProcessA(NULL, &commandBuf[0], NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, ".", &si, &pi)) {
		::WaitForSingleObject(pi.hProcess, INFINITE);
		::CloseHandle(pi.hProcess);
		// ���[�V�����f�[�^�ǂݍ���
		FILE* fin = fopen(output.c_str(), "rb");
		if (fin) {
			model->loadMotionData(fin);
			fclose(fin);
		}
	}
}
