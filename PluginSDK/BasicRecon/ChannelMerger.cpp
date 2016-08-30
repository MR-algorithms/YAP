#include "stdafx.h"
#include "ChannelMerger.h"
#include "Interface/Client/DataHelper.h"
#include <utility>

using namespace Yap;
using namespace std;

CChannelMerger::CChannelMerger(void) :
	ProcessorImpl(L"ChannelMerger")
{
	AddOutputPort(L"Output", 2, DataTypeFloat);
	AddInputPort(L"Input", 2, DataTypeFloat);

	AddProperty(PropertyInt, L"ChannelCount", L"ͨ����");
	AddProperty(PropertyInt, L"ChannelSwitch", L"ͨ������ָʾֵ");
}

CChannelMerger::CChannelMerger( const CChannelMerger& rhs )
	: ProcessorImpl(rhs)
{

}

CChannelMerger::~CChannelMerger(void)
{
}

IProcessor* CChannelMerger::Clone()
{
	try
	{
		auto processor = new CChannelMerger(*this);
		return processor;
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

bool CChannelMerger::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && data->GetData() != nullptr);
	assert(GetInputPorts()->Find(name) != nullptr);

	CDataHelper helper(data);
	
	vector<unsigned int> key = GetKey(data->GetDimensions());
	auto iter = _merge_buffers.find(key);
	if (iter == _merge_buffers.end())
	{
		CDimensionsImpl merge_dimensions(helper.GetDimensionCount() - 1); // ����DimensionChannel��һά

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
		merge_buffer.buffer = YapShared(new CFloatData(&merge_dimensions));

		// merge_buffer.buffer->SetLocalization(CLocalization(*data->GetLocalization()));
		memcpy(merge_buffer.buffer->GetData(), data->GetData(), helper.GetBlockSize(DimensionChannel) * sizeof(float));
		merge_buffer.count = 1;

		auto result = _merge_buffers.insert(make_pair(key, merge_buffer));
		iter = result.first;
	}
	else
	{
		float * merge_cursor = reinterpret_cast<float*>(iter->second.buffer->GetData());
		float * source_cursor = reinterpret_cast<float*>(data->GetData());
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
		bit_number &= (bit_number - 1);   // �������λ��1.
	}   // ���used_channel_count�õ�1�ĸ��������򿪵�ͨ������

	if (iter->second.count == used_channel_count)
	{
		Feed(L"Output", iter->second.buffer.get());
	}

	return true;
}

std::vector<unsigned int> CChannelMerger::GetKey(IDimensions * dimensions) 
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
