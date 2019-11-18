#include "commonmethod.h"
#include <cassert>


void CommonMethod::split_index(const int index, const int width, const int height, int& column, int& row)
{
	assert(index >= 0);
	assert(index < (width *height));

	column = index % width;
	row = index / width;

	assert(column < width);
	assert(row < height);
	assert(index == column + row * width);

	int x = -1;

}


int CommonMethod::GetChannelCountUsed(unsigned int channel_switch)
{
	//method 1:
	//namespace method1 {

	//unsigned int bit_number = channel_switch;
	//unsigned int count_used = 0;
	//for (count_used = 0; bit_number; ++count_used)//does not work.
	//{
	//    bit_number &= (bit_number - 1);   // 消除最低位的1.
	//}   // 最后used_channel_count得到1的个数。即打开的通道总数

	//}



	//method2:
	//namespace method2{

	int count_max = sizeof(channel_switch) * 8;
	int count_used2 = 0;
	for (int channel_index = 0; channel_index < count_max; channel_index++)
	{
		if (channel_switch & (1 << channel_index))
		{
			count_used2++;
		}
	}
	//}

	return count_used2;
}


bool CommonMethod::IsChannelOn(int channel_switch, int channel_index)
{
	bool on = false;
	if (channel_switch & (1 << channel_index))
	{
		on = true;
	}
	return on;
}
