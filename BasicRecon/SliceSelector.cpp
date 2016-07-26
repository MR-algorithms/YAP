#include "stdafx.h"
#include "SliceSelector.h"
#include "DataHelper.h"
#include <complex>
#include "../interface/DataImp.h"
#include "..\Interface\YapInterfaces.h"

using namespace std;
using namespace Yap;

CSliceSelector::CSliceSelector(void):
	CProcessorImp(L"SliceSelector")
{
	AddInputPort(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutputPort(L"Output", YAP_ANY_DIMENSION, DataTypeComplexFloat);

	AddProperty(PropertyInt, L"SliceIndex", L"The index of the slice you want to get.");
	SetInt(L"SliceIndex", 0);
}

Yap::CSliceSelector::CSliceSelector(const CSliceSelector & rhs)
	: CProcessorImp(rhs)
{
}


CSliceSelector::~CSliceSelector()
{
}

IProcessor * Yap::CSliceSelector::Clone()
{
	try
	{
		auto processor = new CSliceSelector(*this);
		return processor;
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

bool Yap::CSliceSelector::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && data->GetData() != nullptr);
	assert(GetInputPortEnumerator()->GetPort(name) != nullptr);

	int slice_index = GetInt(L"SliceIndex");

	CDataHelper input_data(data);
	CDimensionsImp data_dimentions(data->GetDimensions());
	unsigned int slice_block_size = input_data.GetBlockSize(DimensionSlice);
	if (data_dimentions.GetDimensionCount() <= 3)
	{		
		data_dimentions.ModifyDimension(DimensionSlice, 1, slice_index);
		auto output = CSmartPtr<CComplexFloatData>(new CComplexFloatData(reinterpret_cast<std::complex<float>*>(data->GetData())
			+ slice_index * slice_block_size, data_dimentions));
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
			memcpy(slice_channel_data + i * slice_block_size, data + slice_index * slice_block_size + i * channel_block_size, 
				slice_block_size * sizeof(complex<float>));
		}
		CDimensionsImp dimensions;
		dimensions(DimensionReadout, 0U, input_data.GetWidth())
			(DimensionPhaseEncoding, 0U, input_data.GetHeight())
			(DimensionSlice, slice_index, 1)
			(Dimension4, 0U, input_data.GetDim4())
			(DimensionChannel, 0U, input_data.GetCoilCount());
		CSmartPtr<CComplexFloatData> outdata(new CComplexFloatData(
			slice_channel_data, dimensions, nullptr, true));
		Feed(L"Output", outdata.get());
	}

	return true;
}
