//
// Copyright (c) 2015 Yoshiaki JITSUKAWA
//
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
//

#pragma once

#include <stdint.h>
#include <vector>
#include <JAFObj.h>

namespace MOEGUL
{
	//-------------------------------------
	// GLObj: Texture, Shader などを
	//        管理するクラスの基底クラス
	//-------------------------------------
	class GLObj {
		MAKE_NONCOPYABLE(GLObj);
	public:
		uint32_t id() const { return m_id; }
	private:
		GLObj() = delete;
	protected:
		GLObj(uint32_t id) : m_id(id) { }
		uint32_t m_id;
	};

#define DEF_GLOBJ(Name) \
	class Name##Obj : public GLObj { \
	public: \
		friend class GL; \
		typedef std::shared_ptr<Name##Obj> Ptr; \
		Name##Obj(uint32_t id) : GLObj(id) { } \
		~Name##Obj(); \
	private: \
		Name##Obj(const Name##Obj&) = delete; \
		Name##Obj& operator=(const Name##Obj&) = delete; \
		static Ptr create(uint32_t id) { return std::make_shared<Name##Obj>(id); } \
	}; \
	typedef Name##Obj::Ptr Name;

	//-------------------------------------
	// GLParam: glGet* の返り値として使う
	//-------------------------------------
	union GLParam {
		int32_t i;
		float f;
	};
	typedef std::vector<GLParam> GLParams;
}
