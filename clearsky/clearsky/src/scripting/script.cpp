/*
*(C)2011 Symplatonix Software
*/

#include "stdafx.h"
#include "scripting/script.h"

namespace clearengine
{
	/*Script::Script()
	{
		this->m_luaState = lua_open();

		//open all standard libraries
		luaL_openlibs(this->m_luaState);

		//init luabind
		luabind::open(this->m_luaState);
	}

	Script::~Script()
	{
		lua_close(this->m_luaState);
	}*/

	/*int Script::runFile(const char *filename)
	{
		return luaL_dofile(this->m_luaState, filename);
	}

	int Script::runString(const char *string)
	{
		
		return luaL_dostring(this->m_luaState, string);
	}*/

	/*template<class F>
	int Script::registerFunction(const char *name, F function)
	{
		luabind::module(this->m_luaState)
		{
			luabind::def(name, function);
		};
	}*/
}