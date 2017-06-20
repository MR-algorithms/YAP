/********************************************************************
Filename: 	StringHelper.h
Created:	2009/11/27
Author:		Yang Guang, gyang@phy.ecnu.edu.cn
Zhang Chengxiu, angcx@126.com
Purpose:

All right reserved. 2009 -

Shanghai Key Laboratory of Magnetic Resonance,
East China Normal University
*********************************************************************/

#ifndef StringHelper_h__20091127
#define StringHelper_h__20091127

#include <string>

template <typename StringType>
const StringType& make_upper(StringType& my_string)
{
	for (StringType::size_type i = 0; i < my_string.length(); ++i)
	{
		if (my_string[i] >= L'a' && my_string[i] <= L'z')
			my_string[i] -= L'a' - L'A';
	}

	return my_string;
}

template <typename StringType>
const StringType& make_lower(StringType& my_string)
{
	for (StringType::size_type i = 0; i < my_string.length(); ++i)
	{
		if (my_string[i] >= L'A' && my_string[i] <= L'Z')
			my_string[i] += L'a' - L'A';
	}

	return my_string;
}

inline const std::wstring& trim_left(std::wstring& my_string, const wchar_t * white_spaces = L" \t\n\r")
{
	auto begin = my_string.find_first_not_of(white_spaces);
	if (begin != std::wstring::npos)
	{
		my_string = my_string.substr(begin);
	}

	return my_string;
}

inline const std::wstring& trim_right(std::wstring& my_string, const wchar_t * white_spaces = L" \t\n\r")
{
	auto end = my_string.find_last_not_of(white_spaces);
	if (end != std::wstring::npos)
	{
		my_string = my_string.substr(0, end + 1);
	}
	
	return my_string;
}

inline const std::wstring& trim(std::wstring& my_string, const wchar_t * white_spaces = L" \t\n\r")
{
	auto begin = my_string.find_first_not_of(white_spaces);
	if (begin != std::wstring::npos)
	{
		auto end = my_string.find_last_not_of(white_spaces);
		my_string = my_string.substr(begin, end - begin + 1);
	}

	return my_string;
}

#endif // StringHelper_h__20091127

