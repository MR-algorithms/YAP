#include "stdafx.h"
#include "ChannelMerger.h"
#include "Client/DataHelper.h"
#include <utility>
#include "Utilities\CommonMethod.h"
#include "Implement/LogUserImpl.h"
#include <cassert>
#include <time.h>

using namespace Yap;
using namespace std;

ChannelMerger::ChannelMerger(void) :
	ProcessorImpl(L"ChannelMerger")
{
	AddOutput(L"Output", 2, DataTypeFloat);
	AddInput(L"Input", 3, DataTypeFloat);

}

ChannelMerger::ChannelMerger( const ChannelMerger& rhs )
	: ProcessorImpl(rhs)
{
}

ChannelMerger::~ChannelMerger(void)
{
}
bool ChannelMerger::OnTimer()
{
	LOG_TRACE(L"<ChannelMerge> OnTimer::", L"BasicRecon");
	return true;
}

bool ChannelMerger::Input(const wchar_t * name, IData * data)
{
	time_t start = clock();
	assert(data != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	DataHelper helper(data);

	int slice_block_size = helper.GetBlockSize(DimensionSlice);
	unsigned int channel_count;
	unsigned int temp;
	dynamic_cast<Yap::Dimensions*>(data->GetDimensions())->GetDimensionInfo2(
		Yap::DimensionChannel, temp, channel_count);
	//-------------

	Dimensions merge_dimensions(helper.GetDimensionCount() - 1); // 消除DimensionChannel这一维

	DimensionType type = DimensionInvalid;
	unsigned int start_index = 0, length = 0;
	unsigned int dest_dimension_index = 0;

	for (unsigned int i = 0; i < helper.GetDimensionCount(); ++i)
	{
		data->GetDimensions()->GetDimensionInfo(i, type, start_index, length);
		if (type != DimensionChannel)
		{
			merge_dimensions.SetDimensionInfo(dest_dimension_index, type, start_index, length);
			++dest_dimension_index;
		}
	}

	auto output = CreateData<float>(data, &merge_dimensions);
	memset(Yap::GetDataArray<float>(output.get()), 0, helper.GetBlockSize(DimensionSlice) * sizeof(float));
	
	//-----------
	float* source_buffer = Yap::GetDataArray<float>(data);
	for (unsigned int i = 0; i < channel_count; i++)
	{

		float * merge_cursor = Yap::GetDataArray<float>(output.get());

		float * source_cursor = source_buffer + helper.GetBlockSize(DimensionSlice) * i;
		float * source_end = source_cursor + helper.GetBlockSize(DimensionSlice);

		for (; source_cursor < source_end; ++merge_cursor, ++source_cursor)
		{
			*merge_cursor += (*source_cursor) * (*source_cursor);
		}
	
	}

	float * merge_cursor = Yap::GetDataArray<float>(output.get());
	float * merge_end = merge_cursor + helper.GetBlockSize(DimensionSlice);
	//
	auto result = std::minmax_element(merge_cursor, merge_end);
	decltype(*result.first) min_data = *result.first;
	decltype(*result.first) max_data = *result.second;
	min_data = sqrt(min_data);
	max_data = sqrt(max_data);
	double c = 4095 / (max_data - min_data);
	double d = -4095 * min_data / (max_data - min_data);
	//

	for (; merge_cursor < merge_end; ++merge_cursor)
	{
		*merge_cursor = sqrt(*merge_cursor) * c + d;
	}
		
	Log(float(clock()-start));
	
	Feed(L"Output", output.get());
	

	return true;
}

void ChannelMerger::Log(float ms)
{
	wstringstream wss;
	wss << L"<ChannelMerger>: elapsed time =  " << ms << L"ms";
	wstring ws;
	ws = wss.str(); //或 ss>>strtEST;
	LOG_TRACE(ws.c_str(), L"BasicRecon");

}

//这是原来的代码：具有通道数据收集功能和通道合并功能，输入输出数据都是二维的。
/*
bool ChannelMerger::Input(const wchar_t * name, IData * data)
{
	time_t start = clock();
	assert(data != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	DataHelper helper(data);
	assert(HasChannelDimension(data->GetDimensions()));
	vector<unsigned int> key = GetKey(data->GetDimensions());
	auto iter = _merge_buffers.find(key);
	if (iter == _merge_buffers.end())
	{
		Dimensions merge_dimensions(helper.GetDimensionCount() - 1); // 消除DimensionChannel这一维

		DimensionType type = DimensionInvalid;
		unsigned int start_index = 0, length = 0;
		unsigned int dest_dimension_index = 0;

		for (unsigned int i = 0; i < helper.GetDimensionCount(); ++i)
		{
			data->GetDimensions()->GetDimensionInfo(i, type, start_index, length);
			if (type != DimensionChannel)
			{
				merge_dimensions.SetDimensionInfo(dest_dimension_index, type, start_index, length);
				++dest_dimension_index;
			}
		}

		MergeBuffer merge_buffer;
		merge_buffer.buffer = CreateData<float>(data, &merge_dimensions);

		// merge_buffer.buffer->SetLocalization(CLocalization(*data->GetLocalization()));
		auto * data_array = Yap::GetDataArray<float>(data);
		vector<float> module_data(helper.GetBlockSize(DimensionSlice));
		memcpy(module_data.data(), data_array, helper.GetBlockSize(DimensionSlice) * sizeof(float));
		for (unsigned int i = 0; i < module_data.size(); ++i)
		{
			module_data[i] *= module_data[i];
		}

		memcpy(merge_buffer.buffer->GetData(), module_data.data(), helper.GetBlockSize(DimensionSlice) * sizeof(float));
		merge_buffer.count = 1;

		auto result = _merge_buffers.insert(make_pair(key, merge_buffer));
		iter = result.first;
	}
	else
	{
		float * merge_cursor = reinterpret_cast<float*>(iter->second.buffer->GetData());

		auto * source_data_array = Yap::GetDataArray<float>(data);
		vector<float> source_data_vector(helper.GetBlockSize(DimensionSlice));
		memcpy(source_data_vector.data(), source_data_array, helper.GetBlockSize(DimensionSlice) * sizeof(float));
		++iter->second.count;
		for (unsigned int i = 0; i < source_data_vector.size(); ++i, ++merge_cursor)
		{
			*merge_cursor += (source_data_vector[i] * source_data_vector[i]);
		}
	}
	
	//float * merge_cursor = reinterpret_cast<float*>(iter->second.buffer->GetData());
	//float * merge_end = merge_cursor + helper.GetBlockSize(DimensionSlice);

	//for (; merge_cursor < merge_end; ++merge_cursor)
	//{
	//	*merge_cursor = sqrt(*merge_cursor);
	//}
	

// 	unsigned int bit_number = GetInt(L"ChannelSwitch");
// 	unsigned int used_channel_count = 0;
// 	for (used_channel_count = 0; bit_number; ++used_channel_count)
// 	{
// 		bit_number &= (bit_number - 1);   // 消除最低位的1.
// 	}   // 最后used_channel_count得到1的个数。即打开的通道总数
	
	VariableSpace variable(data->GetVariables());
	
	int channel_count = variable.Get<int>(L"channel_count");
	if (iter->second.count == channel_count)
	{
		//xhb.
		float * merge_cursor = reinterpret_cast<float*>(iter->second.buffer->GetData());
		float * merge_end = merge_cursor + helper.GetBlockSize(DimensionSlice);

		for (; merge_cursor < merge_end; ++merge_cursor)
		{
			*merge_cursor = sqrt(*merge_cursor);
		}
		//
		time_t end = clock();
		int ms = float(end - start);// / CLOCKS_PER_SEC;
		//Log
		wstringstream wss;
		wss << L"Channel merger: elapsed time =  " << ms << L"ms";
		wstring ws;
		ws = wss.str(); //或 ss>>strtEST;
		LOG_TRACE(ws.c_str(), L"BasicRecon");
		
		Feed(L"Output", iter->second.buffer.get());
	}

	return true;
}

std::vector<unsigned int> ChannelMerger::GetKey(IDimensions * dimensions) 
{
	std::vector<unsigned int> result;
	for (unsigned int i = 0; i < dimensions->GetDimensionCount(); ++i)
	{
		DimensionType type = DimensionInvalid;
		unsigned int start_index = 0;
		unsigned int length = 0;

		dimensions->GetDimensionInfo(i, type, start_index, length);
		if (type == DimensionChannel)
		{
			result.push_back(0);
		}
		else
		{
			result.push_back(start_index);
		}
	}

	return result;
}

bool ChannelMerger::HasChannelDimension(IDimensions *dimensions) const 
{
	bool  result = false;

	for (unsigned int i = 0; i < dimensions->GetDimensionCount(); ++i)
	{
		DimensionType type = DimensionInvalid;
		unsigned int start_index = 0;
		unsigned int length = 0;

		dimensions->GetDimensionInfo(i, type, start_index, length);
		if (DimensionChannel == type)
		{
			result = true;
		}
	}
	return result;
}

*/

