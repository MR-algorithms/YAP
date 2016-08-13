#ifndef imageProcessing_h__20160814
#define imageProcessing_h__20160814

#pragma once

template<typename T>
void hflip(T * image, size_t width, size_t height)
{
	T temp;
	for (size_t row = 0; row < height; ++row)
	{
		T * begin = image + width * row;
		T * end = image + width * (row + 1) - 1;
		while (begin < end)
		{
			temp = *begin;
			*begin++ = *end;
			*end-- = temp;
		}
	}
}
#endif // imageProcessing_h__