//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include "stringify.h"

namespace
{
	template <typename> class ScopeExit;

	template <typename Func>
	ScopeExit<Func> createScopeExit(Func func) { return ScopeExit<Func>(func); }

	template <typename Func>
	class ScopeExit {
		friend ScopeExit<Func> createScopeExit<Func>(Func);
	public:
		~ScopeExit() { m_func(); }
	private:
		ScopeExit(Func func) : m_func(func) { }
		Func m_func;
	};
}

#define SCOPE_EXIT(code) auto JOINSTR(se, __LINE__) = createScopeExit([&]() code)
