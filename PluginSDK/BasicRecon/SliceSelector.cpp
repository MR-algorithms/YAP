#include "SliceSelector.h"
#include "Client/DataHelper.h"
#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"

#include <complex>

using namespace std;
using namespace Yap;

SliceSelector::SliceSelector(void):
	ProcessorImpl(L"SliceSelector")
{
	LOG_TRACE(L"SliceSelector constructor called.", L"BasicRecon");
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexFloat);

	AddProperty<int>(L"SliceIndex", 3, L"The index of the slice you want to get.");
}

Yap::SliceSelector::SliceSelector(const SliceSelector & rhs)
	: ProcessorImpl(rhs)
{
	LOG_TRACE(L"SliceSelector constructor called.", L"BasicRecon");
}

SliceSelector::~SliceSelector()
{
	LOG_TRACE(L"SliceSelector destructor called.", L"BasicRecon");
}

bool Yap::SliceSelector::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && Yap::GetDataArray<complex<float>>(data) != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	int slice_index = GetProperty<int>(L"SliceIndex");

	DataHelper input_data(data);
	Dimensions data_dimentions(data->GetDimensions());
	unsigned int slice_block_size = input_data.GetBlockSize(DimensionSlice);
	if (input_data.GetActualDimensionCount() <= 3)
	{		
		data_dimentions.SetDimension(DimensionSlice, 1, slice_index);
		auto output = CreateData<complex<float>>(data, 
			Yap::GetDataArray<complex<float>>(data)
			+ slice_index * slice_block_size, data_dimentions);

		Feed(L"Output", output.get());
	}
	else
	{
		Dimension channel_dimention = input_data.GetDimension(DimensionChannel);
		unsigned int channel_block_size = input_data.GetBlockSize(DimensionChannel);
		complex<float>* slice_channel_data = nullptr;
		try
		{
			slice_channel_data = new complex<float>[input_data.GetWidth() * input_data.GetHeight() * input_data.GetCoilCount()];
		}
		catch(bad_alloc&)
		{
			return false;
		}	
		for (unsigned int i = channel_dimention.start_index; i < channel_dimention.start_index + channel_dimention.length; ++i)
		{
			memcpy(slice_channel_data + i * slice_block_size, Yap::GetDataArray<complex<float>>(data) + slice_index * slice_block_size + i * channel_block_size, 
				slice_block_size * sizeof(complex<float>));
		}
		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, input_data.GetWidth())
			(DimensionPhaseEncoding, 0U, input_data.GetHeight())
			(DimensionSlice, slice_index, 1)
			(Dimension4, 0U, input_data.GetDim4())
			(DimensionChannel, 0U, input_data.GetCoilCount());
		auto outdata = CreateData<complex<float>>(data, slice_channel_data, dimensions, nullptr, true);

		Feed(L"Output", outdata.get());
	}

	return true;
}
