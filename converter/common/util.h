//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#ifdef NDEBUG
#define FATAL(...) do { _ftprintf(stderr, __VA_ARGS__); Sleep(3 * 1000); exit(-1); } while (0)
#else
#define FATAL(...) do { _ftprintf(stderr, __VA_ARGS__); Sleep(3 * 1000); throw; } while (0)
#endif
#define INFO(...) do { _ftprintf(stderr, __VA_ARGS__); } while (0)

namespace Util
{
	class Reader {
	public:
		Reader(FILE* fp) : m_fp(fp)
		{
		}
		~Reader()
		{
		}

		template <typename T>
		bool tryRead(T* buffer, size_t count)
		{
			assert(buffer);
			size_t read = fread(buffer, sizeof(T), count, m_fp);
			return read == count;
		}

		template <typename T>
		void read(T* buffer, size_t count)
		{
			if (!tryRead(buffer, count)) {
				FATAL(_T("Corrupted data\n"));
			}
		}

		template <typename T>
		T read()
		{
			T t;
			read<T>(&t, 1);
			return t;
		}

		bool trySkip(size_t len)
		{
			return fseek(m_fp, len, SEEK_CUR) == 0;
		}

		void skip(size_t len)
		{
			if (!trySkip(len)) {
				FATAL(_T("Corrupted data\n"));
			}
		}

	private:
		FILE* m_fp;
	};

	typedef std::basic_string<_TCHAR> tstring;
	tstring getBaseDir(const _TCHAR* path)
	{
		_TCHAR drive[MAX_PATH] = _T("");
		_TCHAR dir[MAX_PATH] = _T("");
		_TCHAR fname[MAX_PATH] = _T("");
		TCHAR ext[MAX_PATH] = _T("");
		_tsplitpath_s(path, drive, MAX_PATH, dir, MAX_PATH, fname, MAX_PATH, ext, MAX_PATH);

		tstring result = drive;
		result += dir;
		return result;
	}

	tstring getFname(const _TCHAR* path)
	{
		_TCHAR drive[MAX_PATH] = _T("");
		_TCHAR dir[MAX_PATH] = _T("");
		_TCHAR fname[MAX_PATH] = _T("");
		TCHAR ext[MAX_PATH] = _T("");
		_tsplitpath_s(path, drive, MAX_PATH, dir, MAX_PATH, fname, MAX_PATH, ext, MAX_PATH);

		tstring result = fname;
		return result;
	}


	tstring toTstring(const std::string& str)
	{
		int wclen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), 0, 0);
		wchar_t *wcbuf = new wchar_t[wclen];
		wclen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), wcbuf, wclen);
#ifdef UNICODE
		tstring tstr(wcbuf, wclen);
#else
#error FIMXE
		tstring tstr();
#endif
		delete[] wcbuf;

		return tstr;
	}

	tstring getTempDir()
	{
		_TCHAR dir[MAX_PATH];
		DWORD len = GetTempPath(MAX_PATH, dir);
		return tstring(dir, len);
	}

} // Util
