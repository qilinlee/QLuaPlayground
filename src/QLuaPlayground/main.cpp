#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "QLuaPlayground.h"
#include "Util/StringUtil.h"
#include "IQLPScript.h"
#include "TraverseTable.h"

std::string strLuaSourcePath;
std::string strCurWorkPath;
std::string strLuaSourceRequirePattern;

std::vector<std::string> LuaTests;

void EnumLuaTests(const std::string& luaSourcePath)
{
	std::filesystem::path p(luaSourcePath);
	if (!exists(p))
		return;

	std::filesystem::directory_entry entry(p);
	if (entry.status().type() != std::filesystem::file_type::directory)
		return;
	std::filesystem::directory_iterator listFiles(p);
	for (auto& it : listFiles)
	{
		std::filesystem::path p = it.path();
		std::filesystem::path f = p.filename();
		const wchar_t* filename = f.c_str();
		std::string strFileName = QLP::WStr2Str(filename);
		QLP::ToLowerCase(strFileName);
		if (strFileName._Starts_with("test"))
		{
			LuaTests.push_back(strFileName);
		}
	}

	std::stable_sort(LuaTests.begin(), LuaTests.end());
}

void OutputInfo(const std::string& luaSourcePath)
{
	if (LuaTests.empty())
	{
		std::cout << "There is no test." << std::endl;
		return;
	}

	std::cout << "There are " << LuaTests.size() << " tests:" << std::endl;
	int index = 1;
	for (auto& it : LuaTests)
	{
		std::cout << std::setw(8) << index << "." << it << std::endl;
		index++;
	}
	std::cout << "input number between 1 ~ " << LuaTests.size() << ":" << std::endl;
}

void DoTest(lua_State* L, const std::string& testPath)
{
	if (!L)
	{
		return;
	}

	int result = luaL_loadfile(L, testPath.c_str());
	if (result)
	{
		return;
	}

	result = lua_pcall(L, 0, 1, 0);
	if (result)
	{
		const char* pErrorMsg = lua_tostring(L, -1);
		std::cout << "Error:" << pErrorMsg << std::endl;
		return;
	}

	if (lua_istable(L, -1))
	{
		lua_pushstring(L, "Do");
		lua_gettable(L, -2);

		lua_pushvalue(L, -2);
		lua_pcall(L, 1, 0, 0);
	}
}

int main()
{
	std::filesystem::path cwd = std::filesystem::current_path();
	
	std::wstring wstrCurWorkPath = cwd.c_str();
	std::wcout << wstrCurWorkPath << std::endl;

	strCurWorkPath = QLP::WStr2Str(wstrCurWorkPath);
	std::cout << "Current work path:" << strCurWorkPath << std::endl;
	strCurWorkPath = strCurWorkPath.substr(0, strCurWorkPath.find_last_of('\\'));

	strLuaSourcePath = strCurWorkPath + "\\LuaSource\\";
	strLuaSourceRequirePattern = strLuaSourcePath + "?.lua";
	std::cout << "Lua source path:" << strLuaSourceRequirePattern << std::endl;

	QLP::LuaSourcePath = strLuaSourcePath;
	QLP::ProjectPath = strCurWorkPath;

	if (!QLP::InitLuaEnv(strLuaSourceRequirePattern.c_str()))
	{
		std::cout << "Lua env init failed!!!" << std::endl;
		return -1;
	}
	
	EnumLuaTests(strLuaSourcePath);

	QLPTraverseTable sampleTraverseTable;
	QLP::CSamples.push_back(&sampleTraverseTable);
	sampleTraverseTable.RegisteLuaEnv();

	while (true)
	{
		OutputInfo(strLuaSourcePath);
		
		if (LuaTests.empty())
			break;

		int indexSelected = 0;
		std::cin >> indexSelected;
		indexSelected--;
		if (0 <= indexSelected && indexSelected < LuaTests.size())
		{
			std::string pathTest = strLuaSourcePath + LuaTests[indexSelected];
			DoTest(QLP::GetLuaState(), pathTest);
		}
		else
		{
			std::cout << "Invalid nubmer" << std::endl;
		}

		std::cout << std::endl << std::endl;
	}


	return 0;
}