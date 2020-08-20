#include "stdafx.h"
#include "ChannelImageDataCollector.h"
#include "Client/DataHelper.h"

#include "Implement/LogUserImpl.h"

using namespace Yap;
using namespace std;

Yap::ChannelImageDataCollector::ChannelImageDataCollector(void):
	ProcessorImpl(L"ChannelImageDataCollector")
{
	AddInput(L"Input", 2, DataTypeFloat);
	AddOutput(L"Output", 3, DataTypeFloat);
}


Yap::ChannelImageDataCollector::ChannelImageDataCollector(const ChannelImageDataCollector& rhs):
	ProcessorImpl(rhs)
{
}


Yap::ChannelImageDataCollector::~ChannelImageDataCollector(void)
{
}

bool Yap::ChannelImageDataCollector::Input(const wchar_t * name, IData * data)
{

	assert(data != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	DataHelper helper(data);
	VariableSpace variable(data->GetVariables());
	int channel_count = variable.Get<int>(L"channel_count");
	

	vector<unsigned int> key = GetKey(data->GetDimensions());
	auto iter = _collector_buffers.find(key);
	if (iter == _collector_buffers.end())
	{
		//
		Dimensions collector_dimensions(helper.GetDimensionCount());
		DimensionType type = DimensionInvalid;
		unsigned int index = 0, length = 0;

		for (unsigned int i = 0; i < helper.GetDimensionCount(); ++i)
		{

			data->GetDimensions()->GetDimensionInfo(i, type, index, length);
			if (type == DimensionChannel)
			{
				collector_dimensions.SetDimensionInfo(i, type, 0, channel_count);
			}
			else
			{
				collector_dimensions.SetDimensionInfo(i, type, index, length);
			}
		}

		CollectorBuffer collector_buffer;
		collector_buffer.buffer = CreateData<float>(data, &collector_dimensions);

		auto * data_array = Yap::GetDataArray<float>(data);
		memcpy(collector_buffer.buffer->GetData(), data_array, helper.GetBlockSize(DimensionSlice) * sizeof(float));
		collector_buffer.count = 1;
		collector_buffer.ready_phasesteps = variable.Get<int>(L"ready_phasesteps");
		assert(collector_buffer.ready_phasesteps >= 0);


		collector_dimensions.GetDimensionInfo2(DimensionSlice, index, length);
		Log(collector_buffer.count - 1, index, collector_buffer.ready_phasesteps);

		auto result = _collector_buffers.insert(make_pair(key, collector_buffer));
		iter = result.first;
	}
	else
	{
		float* collector_cursor = reinterpret_cast<float*>(iter->second.buffer->GetData());

		//-------
		int old_ready_phasesteps = iter->second.ready_phasesteps;
		iter->second.ready_phasesteps = variable.Get<int>(L"ready_phasesteps");

		assert(iter->second.ready_phasesteps >= 0);
		DataHelper helper2(iter->second.buffer.get());
		if (iter->second.ready_phasesteps < old_ready_phasesteps)
		{
		}
		else
		{
			if (iter->second.ready_phasesteps > old_ready_phasesteps)
			{
				iter->second.count = 0;
				memset(collector_cursor, 0, helper2.GetDataSize() * sizeof(float));
			}

			//
			unsigned int index;
			unsigned int length;
			dynamic_cast<Yap::Dimensions*>(data->GetDimensions())->GetDimensionInfo2(
				DimensionSlice, index, length);
			Log(iter->second.count - 1, index, iter->second.ready_phasesteps);
			//

			auto * source_data_array = Yap::GetDataArray<float>(data);
			float * source_cursor = source_data_array;

			memcpy(collector_cursor + iter->second.count * helper.GetBlockSize(DimensionSlice),
				source_cursor, helper.GetBlockSize(DimensionSlice) * sizeof(float));
			++iter->second.count;
		}
		
	}

	if (iter->second.count == channel_count)
	{
		Feed(L"Output", iter->second.buffer.get());
	}

	return true;
}

std::vector<unsigned int> Yap::ChannelImageDataCollector::GetKey(IDimensions * dimensions)
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


void ChannelImageDataCollector::Log(int channel_index, int slice_index, int ready_phasesteps)
{
	wstringstream wss;
	wss << L"<ChannelImageDataCollector>: channel_index = " << channel_index << L"    slice_index = "
		<<slice_index<<L"    ready_phasesteps = "<<ready_phasesteps;
	wstring ws;
	ws = wss.str(); //»ò ss>>strtEST;
	LOG_TRACE(ws.c_str(), L"ChannelImageDataCollector");
}