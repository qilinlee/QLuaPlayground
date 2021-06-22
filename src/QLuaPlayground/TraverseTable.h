#pragma once
#include "IQLPScript.h"
#include "QLuaPlayground.h"

struct lua_State;
class QLPTraverseTable : public QLP::IQLPScript
{
public:
	QLPTraverseTable() = default;
	QLPTraverseTable(const QLPTraverseTable&) = delete;
	QLPTraverseTable(const QLPTraverseTable&&) = delete;

	~QLPTraverseTable();
	virtual void RegisteLuaEnv() override;
	virtual void OnDestroyed() override;
	static int TraverseTableByCAPI(lua_State* L);
};