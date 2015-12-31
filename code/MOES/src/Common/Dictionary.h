//
// Copyright (c) 2016 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#include <stdint.h>
#include <map>
#include <string>

namespace MOES
{
	/// 文字列から整数を引くための辞書。
	/// 整数は非負値が有効値
	struct Dictionary {
	public:
		Dictionary() { }
		~Dictionary() { }

		void add(const char* name, int32_t idx, bool overwrite = true)
		{
			if (idx < 0) {
				return;
			}
			if (!overwrite && m_dict.find(name) == m_dict.end()) {
				return;
			}
			m_dict[name] = idx;
		}

		int32_t lookUp(const char* name) const
		{
			auto it = m_dict.find(name);
			if (it != m_dict.end()) {
				return it->second;
			}
			return -1;
		}

	private:
		std::map<std::string, int32_t> m_dict;
	};
}
