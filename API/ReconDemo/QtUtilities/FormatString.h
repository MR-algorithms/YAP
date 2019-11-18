#pragma once
#include <string>

std::string FormatString(const char* format, ...);
std::wstring FormatStringW(const wchar_t* format, ...);



namespace DebugInfo {

const int flow_type1 = 1;
const int flow_type2 = 2;
const int flow_type3 = 3;
const int flow_type4 = 4;
const int flow_type5 = 5;

void Output(const wchar_t *info_tag, const wchar_t* info, int this_pointer,
	bool first_enter, int flow_type = 0);
std::wstring String2WString(const std::string& s);
std::wstring get_threadinfo();

}
