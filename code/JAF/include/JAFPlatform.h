//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#if defined(WIN32)
#include <windows.h>
typedef HDC JAFPlatformDisplay;
#else
// for Unknown OS
typedef void* JAFPlatformDisplay;
#endif

namespace JAF
{
	typedef ::JAFPlatformDisplay PlatformDisplay;
}
