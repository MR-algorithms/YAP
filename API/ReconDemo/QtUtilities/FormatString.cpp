#include "FormatString.h"
#include <thread>
#include <string>
#include <sstream>
#include <iostream>

#include <cassert>

#include <stdarg.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <QDebug>

//#include <afx.h>

std::string FormatString(const char* format, ...)
{
	std::string result_str;
	va_list ap;
	va_start(ap, format);
	int size = 256;
	while(true)
	{
		std::vector<char> buffer(size);
		int result = vsnprintf(buffer.data(), size, format, ap);
		if (result > 0 && result < size)
		{
			if (result < size)
			{
				buffer[result] = L'\0';
				++result;
			}
			result_str = std::string(buffer.data());
			break;
		}
		size *= 2;
	}
	va_end(ap);
	return result_str;
}

std::wstring FormatStringW(const wchar_t* format, ...)
{
	std::wstring result_str;
	va_list ap;
	va_start(ap, format);
	int size = 256;
	while(true)
	{
		std::vector<wchar_t> buffer(size);
		int result = vswprintf(buffer.data(), size, format, ap);
		if (result > 0 && result < size)
		{
			if (result < size)
			{
				buffer[result] = L'\0';
				++result;
			}
			result_str = std::wstring(buffer.data());
			break;
		}
		size *= 2;
	}
	va_end(ap);
	return result_str;
}



void DebugInfo::Output(const wchar_t *info_tag, const wchar_t* info, int this_pointer, bool first_enter, int flow_type)
{
	
	static int enter_times = 0;
	auto threadid = std::this_thread::get_id();
	std::stringstream ss2;
	threadid._To_text(ss2);
	std::string str;
	ss2 >> str;

	if (first_enter)
	{

        //QDebug()<<FormatSting("%d, :: -------Enter %s--------- ", enter_times, info_tag).c_str();

		
	}

    //QDebug()<<FormatSting("%s : %s", info_tag, info).c_str();
    //QDebug()<<FormatSting("thread id: %s", str.c_str()).c_str();
    //QDebug()<<FormatSting("this pointer: %x", this_pointer).c_str();
	
	
}

std::wstring DebugInfo::get_threadinfo()
{
	std::wstring test1(L"12345");
	std::wstring test2(L"123456");
	
	auto threadid = std::this_thread::get_id();
	std::stringstream ss2;
	threadid._To_text(ss2);

	std::string str;
	ss2 >> str;
	str = std::string("thread id = ") + str;

	return std::wstring(DebugInfo::String2WString(str));



}

std::wstring DebugInfo::String2WString(const std::string& s)
{
	
	/*
	
	std::string strLocale = setlocale(LC_ALL, "");

	size_t len = strlen(s.c_str()) + 1;
	wchar_t* wstr = new wchar_t[len];
	size_t wlen = 0;
	wmemset(wstr, 0, len*sizeof(wchar_t));
	
	const char* src = s.c_str();
	mbstowcs_s(&wlen, wstr, len, src, _TRUNCATE);

	std::wstring wstrResult = wstr;
	delete[]wstr;
	setlocale(LC_ALL, strLocale.c_str());
	return wstrResult;
	*/
	
	size_t convertedChars = 0;
	std::string curLocale = setlocale(LC_ALL, NULL);   //curLocale="C"
	setlocale(LC_ALL, "chs");
	const char* source = s.c_str();
	size_t charNum = sizeof(char)*s.size() + 1;
	wchar_t* dest = new wchar_t[charNum];
	mbstowcs_s(&convertedChars, dest, charNum, source, _TRUNCATE);
	std::wstring result = dest;
	delete[] dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
	
	
}
