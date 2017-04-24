#include "stdafx.h"
#include "ChannelMerger.h"
#include "Interface/Client/DataHelper.h"
#include <utility>

using namespace Yap;
using namespace std;

ChannelMerger::ChannelMerger(void) :
	ProcessorImpl(L"ChannelMerger")
{
	AddOutput(L"Output", 2, DataTypeFloat);
	AddInput(L"Input", 2, DataTypeFloat);

	_properties->Add(VariableInt, L"ChannelCount", L"通道数");
	_properties->Set<int>(L"ChannelCount", 4);
	_properties->Add(VariableInt, L"ChannelSwitch", L"通道开关指示值");
}

ChannelMerger::ChannelMerger( const ChannelMerger& rhs )
	: ProcessorImpl(rhs)
{
}

ChannelMerger::~ChannelMerger(void)
{
}

bool ChannelMerger::Input(const wchar_t * name, IData * data)
{
	assert(data != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	DataHelper helper(data);
	
	vector<unsigned int> key = GetKey(data->GetDimensions());
	auto iter = _merge_buffers.find(key);
	if (iter == _merge_buffers.end())
	{
		Dimensions merge_dimensions(helper.GetDimensionCount() - 1); // 消除DimensionChannel这一维

		DimensionType type = DimensionInvalid;
		unsigned int index = 0, length = 0;
		unsigned int dest_dimension_index = 0;

		for (unsigned int i = 0; i < helper.GetDimensionCount(); ++i)
		{
			data->GetDimensions()->GetDimensionInfo(i, type, index, length);
			if (type != DimensionChannel)
			{
				merge_dimensions.SetDimensionInfo(dest_dimension_index, type, index, length);
				++dest_dimension_index;
			}
		}

		MergeBuffer merge_buffer;
		merge_buffer.buffer = YapShared(new FloatData(&merge_dimensions));

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

	float * merge_cursor = reinterpret_cast<float*>(iter->second.buffer->GetData());
	float * merge_end = merge_cursor + helper.GetBlockSize(DimensionSlice);

	for (; merge_cursor < merge_end; ++merge_cursor)
	{
		*merge_cursor = sqrt(*merge_cursor);
	}

// 	unsigned int bit_number = GetInt(L"ChannelSwitch");
// 	unsigned int used_channel_count = 0;
// 	for (used_channel_count = 0; bit_number; ++used_channel_count)
// 	{
// 		bit_number &= (bit_number - 1);   // 消除最低位的1.
// 	}   // 最后used_channel_count得到1的个数。即打开的通道总数

	if (iter->second.count == _properties->Get<int>(L"ChannelCount"))
	{
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
