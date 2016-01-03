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
/// �R�s�[�s��
///
#define MAKE_NONCOPYABLE(Name) \
	private: \
		Name(const Name&) = delete; \
		Name& operator=(const Name&) = delete;


///
/// unique_ptr �ŊǗ������R�s�[�s�ȃI�u�W�F�N�g
///
#define MAKE_JAFOBJ(Name) \
	MAKE_NONCOPYABLE(Name) \
	public: \
		typedef std::unique_ptr<Name> Ptr; \
	protected: \
		Name();
