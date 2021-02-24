// QLuaPlayground.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "QLuaPlayground.h"


namespace QLP
{
	lua_State* g_lusState = nullptr;
	std::string LuaSourcePath;
	std::string ProjectPath;
	bool InitLuaEnv(const char* path)
	{
		if (g_lusState)
			return false;

		g_lusState = luaL_newstate();
		if (g_lusState)
		{
			
			luaopen_base(g_lusState);
			luaL_openlibs(g_lusState);

			if (path && strlen(path) > 0)
			{
				AddLuaPackagePath(path);
			}
		}

		return true;
	}
	
	lua_State* GetLuaState()
	{
		if (!g_lusState)
		{
			InitLuaEnv();
		}

		return g_lusState;
	}

	void AddLuaPackagePath(const char* path)
	{
		if (!path || !g_lusState)
		{
			return;
		}

		lua_getglobal(g_lusState, "package");
		lua_getfield(g_lusState, -1, "path");
		char finalPath[1024] = {0};
		sprintf_s(finalPath, "%s;%s", lua_tostring(g_lusState, -1), path);
		lua_pushstring(g_lusState, finalPath);
		lua_setfield(g_lusState, -3, "path");
		lua_pop(g_lusState, 2);
	}

	void CloseLuaEnv()
	{
		if (g_lusState)
		{
			lua_close(g_lusState);
		}
	}

}
