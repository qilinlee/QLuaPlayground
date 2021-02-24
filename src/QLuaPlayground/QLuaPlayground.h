#pragma once

#include <string>
#include "lua.hpp"
#pragma comment(lib, "lua.lib")


struct lua_State;

namespace QLP
{
	extern std::string LuaSourcePath;
	extern std::string ProjectPath;
	bool InitLuaEnv(const char* path = nullptr);
	lua_State* GetLuaState();
	void AddLuaPackagePath(const char* path);
	void CloseLuaEnv(); 
}