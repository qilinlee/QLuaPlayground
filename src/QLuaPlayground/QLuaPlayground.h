#pragma once

#include <string>
#include <vector>
#include "lua.hpp"
#pragma comment(lib, "lua.lib")
#include "IQLPScript.h"

struct lua_State;

namespace QLP
{
	extern std::string LuaSourcePath;
	extern std::string ProjectPath;
	extern std::vector<QLP::IQLPScript*> CSamples;
	bool InitLuaEnv(const char* path = nullptr);
	lua_State* GetLuaState();
	void AddLuaPackagePath(const char* path);
	void CloseLuaEnv(); 
	void RegisteCFun(const char* strNameFun,lua_CFunction funcLib);
	void RegisterCLib(const char* strNameLib, const luaL_Reg* lib);
}