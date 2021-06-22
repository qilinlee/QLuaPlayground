// QLuaPlayground.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "QLuaPlayground.h"
#include "IQLPScript.h"

namespace QLP
{
	lua_State* g_lusState = nullptr;
	std::string LuaSourcePath;
	std::string ProjectPath;
	std::vector<IQLPScript*> CSamples;
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

		for (int i = 0; i < CSamples.size(); ++i)
		{
			if (CSamples[i])
			{
				CSamples[i]->RegisteLuaEnv();
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

	void RegisteCFun(const char* strNameFun, lua_CFunction funcLib)
	{
		if (g_lusState)
		{
			lua_register(g_lusState, strNameFun, funcLib);
		}
	}

	void RegisterCLib(const char* strNameLib, const luaL_Reg* lib)
	{
		if (g_lusState)
		{
			luaL_newlib(g_lusState, lib);
		}
	}

}
