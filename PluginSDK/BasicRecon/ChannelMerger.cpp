#include "stdafx.h"
#include "ChannelMerger.h"
#include "Interface/Client/DataHelper.h"
#include <utility>

using namespace Yap;
using namespace std;

ChannelMerger::ChannelMerger(void) :
	ProcessorImpl(L"ChannelMerger")
{
}

ChannelMerger::ChannelMerger( const ChannelMerger& rhs )
	: ProcessorImpl(rhs)
{

}

ChannelMerger::~ChannelMerger(void)
{
}

bool Yap::ChannelMerger::OnInit()
{
	AddOutput(L"Output", 2, DataTypeFloat);
	AddInput(L"Input", 2, DataTypeFloat);

	AddProperty(PropertyInt, L"ChannelCount", L"通道数");
	AddProperty(PropertyInt, L"ChannelSwitch", L"通道开关指示值");

	return true;
}

IProcessor* ChannelMerger::Clone()
{
	return new(nothrow) ChannelMerger(*this);
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
		memcpy(merge_buffer.buffer->GetData(), data_array, helper.GetBlockSize(DimensionChannel) * sizeof(float));
		merge_buffer.count = 1;

		auto result = _merge_buffers.insert(make_pair(key, merge_buffer));
		iter = result.first;
	}
	else
	{
		float * merge_cursor = reinterpret_cast<float*>(iter->second.buffer->GetData());

		auto * source_data_array = Yap::GetDataArray<float>(data);
		float * source_cursor = source_data_array;
		float * source_end = source_cursor + helper.GetBlockSize(DimensionChannel);
		++iter->second.count;
		for (; source_cursor < source_end; ++source_cursor, ++merge_cursor)
		{
			*merge_cursor += *source_cursor;
		}
	}

	unsigned int bit_number = GetInt(L"ChannelSwitch");
	unsigned int used_channel_count = 0;
	for (used_channel_count = 0; bit_number; ++used_channel_count)
	{
		bit_number &= (bit_number - 1);   // 消除最低位的1.
	}   // 最后used_channel_count得到1的个数。即打开的通道总数

	if (iter->second.count == used_channel_count)
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

template<typename T>
T * GetData(IData * data)
{

}