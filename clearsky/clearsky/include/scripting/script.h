/*
*(C)2011 Symplatonix Software
*/

#ifndef _CLEARENGINE_SCRIPT_H_
#define _CLEARENGINE_SCRIPT_H_

#include "lua/luabind/luabind.hpp"

extern "C"
{
	#include "lua/lua.h"
	#include "lua/lauxlib.h"
	#include "lua/lualib.h"
}

#include "core/utils.h"

namespace clearengine
{
	//lua scripting class
	class CLEARENGINE_API Script
	{
		public:
			Script()
			{
				this->m_luaState = lua_open();

				//open all standard libraries
				luaL_openlibs(this->m_luaState);

				//init luabind
				luabind::open(this->m_luaState);
			}

			~Script()
			{
				lua_close(this->m_luaState);
			}

			//run lua file
			int runFile(const char *filename)
			{
				return luaL_dofile(this->m_luaState, filename);
			}

			//run lua code from a string
			int runString(const char *string)
			{
				return luaL_dostring(this->m_luaState, string);
			}
		
			//enable using a c++ function from lua
			template<class F>
			int addFunction(const char *name, F func)
			{
				luabind::module(this->m_luaState)
				[
					luabind::def(name, func)
				];
				
				return 0;
			}

			template<class T>
			int callFunction(const char *name)
			{
				luabind::call_function<T>(this->m_luaState, name);

				return 0;
			}

			template<class ClassType, class MemberType>
			int addClassVariable(const char *className, ClassType, const char *memberName, MemberType memberVariable)
			{
				luabind::module(this->m_luaState)
				[
					luabind::class_<ClassType>(className).def_readwrite(memberName, memberVariable)

				];

				return 0;
			}

			template<class ClassType, class MemberType>
			int addClassFunction(const char *className, ClassType, const char *memberName, MemberType memberFunction)
			{
				className;
				memberName;
				memberFunction;

				luabind::module(this->m_luaState)
				[
					luabind::class_<ClassType>(className).def_readwrite(memberName, memberVariable)
				];

				
				return 0;
			}

			template<class ClassType, class CtorType>
			int addClassConstructor(const char *className)
			{			
				luabind::module(this->m_luaState)
				[
					luabind::class_<ClassType>(className).def(luabind::constructor<CtorType>())
				];
				//luabind::detail::class_rep *rep = luabind::detail::class_registry.find_class(LUABIND_TYPEID(ClassType));
				//printf("class name: %s", rep->name);
				return 0;
			}

			int Script::readInt(const char *name)
			{
				lua_getglobal(this->m_luaState, name);

				return (int)lua_tonumber(this->m_luaState, -1);
			}

			bool Script::readBool(const char *name)
			{
				lua_getglobal(this->m_luaState, name);

				return lua_toboolean(this->m_luaState, -1)!=0;
			}

			std::string Script::readString(const char *name)
			{
				lua_getglobal(this->m_luaState, name);

				return std::string(lua_tostring(this->m_luaState, -1));
			}

			void* getScriptState()
			{
				return (void*)this->m_luaState;
			}

			template<class T>
			void addGlobal(const char *name, T object)
			{
				luabind::globals(this->m_luaState)[name] = object;
			}

		private:
			lua_State *m_luaState;

			std::map<const char*, luabind::detail::class_base*> m_registeredClasses;
	};

}

#endif