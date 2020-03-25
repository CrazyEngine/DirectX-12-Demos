#pragma once

#include <codecvt>
#include <string>

//namespace CrazyBakery
//{
	inline std::wstring StringToWString(const std::string &str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
		return converterX.from_bytes(str);
	}

	inline std::string WStringToString(const std::wstring &str)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
		return converterX.to_bytes(str);
	}

	inline std::wstring GetParent(const std::wstring &path)
	{
		return path.substr(0, path.find_last_of(L"\\/") + 1);
	}
	inline std::string GetParent(const std::string &path)
	{
		return path.substr(0, path.find_last_of("\\/") + 1);
	}
	inline std::string Replace(std::string &str, const std::string &cOld, const char cNew)
	{
		for (int i = 0; i < str.size(); i++)
		{
			for (int j = 0; j < cOld.size(); j++)
				if (str[i] == cOld[j])
				{
					str[i] = cNew;
					break;
				}	
		}
		return str;
	}
	inline std::wstring Replace(std::wstring &str, const std::wstring &cOld, const wchar_t cNew)
	{
		for (int i = 0; i < str.size(); i++)
		{
			for (int j = 0; j < cOld.size(); j++)
				if (str[i] == cOld[j])
				{
					str[i] = cNew;
					break;
				}
		}
		return str;
	}
//}
