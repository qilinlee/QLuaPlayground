#include "TraverseTable.h"
#include "lua.hpp"
#include "QLuaPlayground.h"
#include <iostream>

QLPTraverseTable::~QLPTraverseTable()
{
	OnDestroyed();
}

void QLPTraverseTable::RegisteLuaEnv()
{
	QLP::RegisteCFun("TraverseTable", &QLPTraverseTable::TraverseTableByCAPI);
}

void QLPTraverseTable::OnDestroyed()
{

}

int QLPTraverseTable::TraverseTableByCAPI(lua_State* L)
{
	int numParams = lua_gettop(L);
	if (numParams < 1)
	{
		luaL_error(L, "Invalid parameters:must have one table");
		return 0;
	}

	int type = lua_type(L, -1);
	if (type != LUA_TTABLE)
	{
		luaL_error(L, "Invalid parameters:must have one table");
		return 0;
	}

	std::cout << "traverse table by C API:" << std::endl;
	lua_pushnil(L);
	while (lua_next(L, -2))
	{
		lua_pushvalue(L, -2);
		lua_pushvalue(L, -2);
		std::cout <<lua_tostring(L, -2) << "(" << lua_typename(L, lua_type(L, -4)) << "):" << lua_tostring(L, -1) << "(" << lua_typename(L, lua_type(L, -3)) << ")" << std::endl;
		lua_pop(L, 3);
	}
	std::cout << std::endl;
	return 1;
}
