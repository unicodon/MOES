//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <memory>
#include <stdint.h>

///
/// コピー不可
///
#define MAKE_NONCOPYABLE(Name) \
	private: \
		Name(const Name&) = delete; \
		Name& operator=(const Name&) = delete;


///
/// unique_ptr で管理されるコピー不可なオブジェクト
///
#define MAKE_JAFOBJ(Name) \
	MAKE_NONCOPYABLE(Name) \
	public: \
		typedef std::unique_ptr<Name> Ptr; \
	protected: \
		Name();
