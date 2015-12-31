//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#ifndef NDEBUG
#define DBG_INFO(...) printf(__VA_ARGS__)
#define DBG_ERROR(...) printf("E: %s %d ", __FUNCTION__, __LINE__);printf(__VA_ARGS__)
#else
#define DBG_INFO(...) 
#define DBG_ERROR(...) 
#endif

