//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define JOINSTR_(x, y) x ## y
#define JOINSTR(x, y) JOINSTR_(x, y)
