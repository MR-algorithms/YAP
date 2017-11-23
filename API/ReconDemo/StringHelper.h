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

#endif // StringHelper_h__20091127

