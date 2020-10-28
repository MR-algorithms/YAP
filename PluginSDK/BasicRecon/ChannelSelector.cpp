#include "stdafx.h"
#include "ChannelSelector.h"
#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <complex>

using namespace std;
using namespace Yap;

ChannelSelector::ChannelSelector(void):
	ProcessorImpl(L"ChannelSelector")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexFloat);

	AddProperty<int>(L"ChannelIndex", 3, L"The index of the channel you want to get.");
	
}

Yap::ChannelSelector::ChannelSelector(const ChannelSelector & rhs)
	: ProcessorImpl(rhs)
{
}

ChannelSelector::~ChannelSelector()
{
}
	

bool Yap::ChannelSelector::Input(const wchar_t * name, IData * data)
{
	
	bool is_type_complexf = (Yap::GetDataArray<complex<float>>(data) != nullptr);
	bool is_type_short =	(Yap::GetDataArray<short>(data) != nullptr);

	assert((data != nullptr) && (is_type_complexf || is_type_short));
	assert(Inputs()->Find(name) != nullptr);
		
	int selected_index = GetProperty<int>(L"ChannelIndex");

	DataHelper input_data(data);
	Dimensions data_dimensions(data->GetDimensions());
	//
	unsigned int channel_dimension_count;
	unsigned int channel_index;
	data_dimensions.GetDimensionInfo2(Yap::DimensionChannel, channel_index, channel_dimension_count);
//
	if (channel_dimension_count == 1)
	{		
		if (channel_index == selected_index)
		{
			{//log

				wstring info = wstring(L"<ChannalSelector>") +
					L"-----selected channel index = " + to_wstring(selected_index);
					
				LOG_TRACE(info.c_str(), L"BasicRecon");
			}

			if (is_type_complexf)
			{
				auto output = CreateData<complex<float>>(data,
					Yap::GetDataArray<complex<float>>(data),
					data_dimensions, data);
				//Parameter "channel_count" cannot be saved in dimension.
				AddASingleVarible(output.get(), L"channel_count", 1, DataHelper(output.get()).GetDataType());
				Feed(L"Output", output.get());
			}
			if (is_type_short)
			{
				auto output = CreateData<short>(data,
					Yap::GetDataArray<short>(data),
					data_dimensions, data);
				AddASingleVarible(output.get(), L"channel_count", 1, DataHelper(output.get()).GetDataType());
				Feed(L"Output", output.get());
			}
		}
		
	}
	else
	{
		assert(0 && L"Not Impemented!");
	}
	return true;
}