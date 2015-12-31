//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "JTL.h"

namespace JAF
{
	class File {
	public:
		size_t fread(void *buf, size_t size, size_t n);
	};

	// «—ˆ‚Ìæ‚èŠ·‚¦‚Ì‚½‚ß‚Ìtemplate
	template <typename T>
	size_t fread(void *buf, size_t size, size_t n, T *fp);

	template <>
	inline size_t fread(void *buf, size_t size, size_t n, ::FILE *fp)
	{
		return ::fread(buf, size, n, fp);
	}

	// •„†‚È‚µ32bit®”‚ğ“Ç‚İ‚İ
	template <typename T>
	inline bool readU32(uint32_t &out, T *fp)
	{
		uint32_t x;
		if (fread(&x, sizeof x, 1, fp) == 1) {
			out = x;
			return true;
		}
		return false;
	}

	// ’P¸“x•‚“®¬”“_”‚ğ“Ç‚İ‚İ
	template <typename T>
	inline bool readFloatArray(float *out, unsigned int count, T *fp)
	{
		return fread(out, sizeof(float), count, fp) == count;
	}
}
