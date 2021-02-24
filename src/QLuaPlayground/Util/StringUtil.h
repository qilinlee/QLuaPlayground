#pragma once
#include <string>
#include <clocale>	/// std::setlocale
#include <cstdlib>	/// std::wcstombs, std::mbstowcs
#include <algorithm>

namespace QLP
{
	// convert string to wstring
	inline std::wstring Str2Wide(const std::string& input)
	{
		std::setlocale(LC_CTYPE, "");
		const char* cstr = input.c_str();
#if _WIN32
		std::size_t cstrlen;
		wchar_t* tmp = nullptr;
		mbstowcs_s(&cstrlen, tmp, 0, cstr, 0);

		wchar_t* wcstr = new wchar_t[cstrlen];
		std::wmemset(wcstr, 0, cstrlen);
		mbstowcs_s(&cstrlen, wcstr, cstrlen, cstr, cstrlen);
		std::wstring wstr(wcstr);
#else
		std::size_t cstrlen = std::mbstowcs(nullptr, cstr, 0) + 1;
		wchar_t* wcstr = new wchar_t[cstrlen];
		std::wmemset(wcstr, 0, cstrlen);
		std::mbstowcs(wcstr, cstr, cstrlen);
		std::wstring wstr(wcstr);
#endif

		delete[]wcstr;
		return wstr;

	}

	// convert wstring to string 
	inline std::string WStr2Str(const std::wstring& input)
	{
		std::setlocale(LC_CTYPE, "");
		const wchar_t* wcstr = input.c_str();
#if _WIN32
		std::size_t wcstrlen;
		char* tmp = nullptr;
		wcstombs_s(&wcstrlen, tmp, 0, wcstr, 0);
		
		char* cstr = new char[wcstrlen];
		std::memset(cstr, 0, wcstrlen);
		wcstombs_s(&wcstrlen, cstr, wcstrlen, wcstr, wcstrlen);
		std::string str(cstr);
#else
		std::size_t wcstrlen = std::wcstombs(nullptr, wcstr, 0) + 1;
		char* cstr = new char[wcstrlen];
		std::memset(cstr, 0, wcstrlen);
		std::wcstombs(cstr, wcstr, wcstrlen);
		std::string str(cstr);
#endif
		delete[]cstr;
		return str;
	}

	inline void ToLowerCase(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {return std::tolower(c); });
	}
}