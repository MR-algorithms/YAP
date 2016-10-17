#include "stdafx.h"
#include "ChannelDataCollector.h"
#include "Interface/Client/DataHelper.h"


using namespace Yap;
using namespace std;

ChannelDataCollector::ChannelDataCollector(void):
	ProcessorImpl(L"ChannelDataCollector")
{
}


Yap::ChannelDataCollector::ChannelDataCollector(const ChannelDataCollector& rhs):
	ProcessorImpl(rhs)
{

}


ChannelDataCollector::~ChannelDataCollector(void)
{
}

bool Yap::ChannelDataCollector::OnInit()
{
	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 3, DataTypeComplexFloat);
	AddProperty(PropertyInt, L"ChannelCount", L"The total channel count.");
	SetInt(L"ChannelCount", 4);

	return true;
}

IProcessor * Yap::ChannelDataCollector::Clone()
{
	return new(nothrow) ChannelDataCollector(*this);
}

bool Yap::ChannelDataCollector::Input(const wchar_t * name, IData * data)
{
	assert(data != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	DataHelper helper(data);

	vector<unsigned int> key = GetKey(data->GetDimensions());
	auto iter = _collector_buffers.find(key);
	if (iter == _collector_buffers.end())
	{
		Dimensions collector_dimensions(helper.GetDimensionCount());
		DimensionType type = DimensionInvalid;
		unsigned int index = 0, length = 0;

		for (unsigned int i = 0; i < helper.GetDimensionCount(); ++i)
		{

			data->GetDimensions()->GetDimensionInfo(i, type, index, length);
			if (type == DimensionChannel)
			{
				collector_dimensions.SetDimensionInfo(i, type, index, GetInt(L"ChannelCount"));
			}
			else
			{
				collector_dimensions.SetDimensionInfo(i, type, index, length);
			}
		}

		CollectorBuffer collector_buffer;
		collector_buffer.buffer = YapShared(new ComplexFloatData(&collector_dimensions));

		auto * data_array = Yap::GetDataArray<complex<float>>(data);
		memcpy(collector_buffer.buffer->GetData(), data_array, helper.GetBlockSize(DimensionSlice) * sizeof(complex<float>));
		collector_buffer.count = 1;

		auto result = _collector_buffers.insert(make_pair(key, collector_buffer));
		iter = result.first;
	}
	else
	{
		complex<float> * collector_cursor = reinterpret_cast<complex<float>*>(iter->second.buffer->GetData());

		auto * source_data_array = Yap::GetDataArray<complex<float>>(data);
		complex<float> * source_cursor = source_data_array;
		
		memcpy(collector_cursor + iter->second.count * helper.GetBlockSize(DimensionSlice), source_cursor, helper.GetBlockSize(DimensionSlice) * sizeof(complex<float>));
		++iter->second.count;
	}


	if (iter->second.count == GetInt(L"ChannelCount"))
	{
		Feed(L"Output", iter->second.buffer.get());
	}

	return true;
}

std::vector<unsigned int> Yap::ChannelDataCollector::GetKey(IDimensions * dimensions)
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
