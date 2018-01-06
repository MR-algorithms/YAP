#include "stdafx.h"
#include "ChannelIterator.h"
#include "Client/DataHelper.h"
#include <complex>
#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"

using namespace std;
using namespace  Yap;

ChannelIterator::ChannelIterator():
	ProcessorImpl(L"ChannelIterator")
{
	LOG_TRACE(L"ChannelIterator constructor called.", L"BasicRecon");
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);

	AddProperty<int>(L"SliceIndex", 0, L"The index of the slice you want to get.");
}

ChannelIterator::ChannelIterator(const ChannelIterator& rhs)
	: ProcessorImpl(rhs)
{
	LOG_TRACE(L"ChannelIterator copy constructor called.", L"BasicRecon");
}

ChannelIterator::~ChannelIterator()
{
	LOG_TRACE(L"ChannelIterator destructor called.", L"BasicRecon");
}

bool Yap::ChannelIterator::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && (Yap::GetDataArray<complex<float>>(data) != nullptr));
	assert(Inputs()->Find(name) != nullptr);

	DataHelper helper(data);

	unsigned int slice_block_size = helper.GetBlockSize(DimensionSlice);
	unsigned int slice_index = GetProperty<int>(L"SliceIndex");

	Dimension channel_dimension = helper.GetDimension(DimensionChannel);
	assert(channel_dimension.type == DimensionChannel);

	for (unsigned int i = channel_dimension.start_index; i < channel_dimension.start_index + channel_dimension.length; ++i)
	{
		Dimensions channel_slice_data_dimensions(data->GetDimensions());
		channel_slice_data_dimensions.SetDimension(DimensionSlice, 1, slice_index);
		channel_slice_data_dimensions.SetDimension(DimensionChannel, 1, i);

		auto output = CreateData<complex<float>>(data,
			Yap::GetDataArray<complex<float>>(data) + i * slice_block_size, channel_slice_data_dimensions);

		Feed(L"Output", output.get());
	}

	return true;
}
