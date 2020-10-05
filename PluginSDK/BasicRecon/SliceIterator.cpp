#include "stdafx.h"
#include "SliceIterator.h"

#include "Client/DataHelper.h"
#include <complex>
#include "Implement/LogUserImpl.h"

using namespace Yap;
using namespace std;

SliceIterator::SliceIterator(void) :
	ProcessorImpl(L"SliceIterator")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat | DataTypeUnsignedShort);
	AddOutput(L"Output", 2, DataTypeComplexFloat | DataTypeUnsignedShort);
}

SliceIterator::SliceIterator( const SliceIterator& rhs)
	: ProcessorImpl(rhs)
{

}


SliceIterator::~SliceIterator(void)
{
}

bool SliceIterator::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && ((Yap::GetDataArray<complex<float>>(data) != nullptr) || Yap::GetDataArray<unsigned short>(data) != nullptr));
	assert(Inputs()->Find(name) != nullptr);

	DataHelper helper(data);

	unsigned int slice_block_size = helper.GetBlockSize(DimensionSlice);

	Dimension slice_dimension = helper.GetDimension(DimensionSlice);
	assert(slice_dimension.type == DimensionSlice);
	
	for (unsigned int i = slice_dimension.start_index; i < slice_dimension.start_index + slice_dimension.length; ++i)
	{
		Dimensions slice_data_dimensions(data->GetDimensions());
		slice_data_dimensions.SetDimensionInfo2(DimensionSlice, i, 1);
		
		unsigned int length;
		unsigned int index;
		dynamic_cast<Yap::Dimensions*>(data->GetDimensions())->GetDimensionInfo2(Yap::DimensionSlice, index, length);
	

	

		if (helper.GetDataType() == DataTypeComplexFloat)
		{
			auto output = CreateData<complex<float>>(data,
				Yap::GetDataArray<complex<float>>(data) + i * slice_block_size, slice_data_dimensions, data);

			AddASingleVarible(output.get(), L"slice_count", length, DataHelper(output.get()).GetDataType());
			{
				Yap::VariableSpace variable(output->GetVariables());
				int ready_phasesteps = variable.Get<int>(L"ready_phasesteps");
	
				unsigned int length;
				unsigned int index;
				dynamic_cast<Yap::Dimensions*>(output->GetDimensions())->GetDimensionInfo2(Yap::DimensionSlice, index, length);
				int slice_index = index;
				dynamic_cast<Yap::Dimensions*>(output->GetDimensions())->GetDimensionInfo2(Yap::DimensionChannel, index, length);
				int channel_index = index;

				wstring info = wstring(L"<SliceIterator>") + L"::Feed -----channel_index = " + to_wstring(channel_index)
					+ L"----slice_index = " + to_wstring(slice_index)
					+ L"----ready_phasesteps = " + to_wstring(ready_phasesteps) + L"-------------";
				LOG_TRACE(info.c_str(), L"BasicRecon2");
			}

			Feed(L"Output", output.get());
		}
		else
		{
			auto output = CreateData<unsigned short>(data,
				Yap::GetDataArray<unsigned short>(data) + i * slice_block_size, slice_data_dimensions, data);

			AddASingleVarible(output.get(), L"slice_count", length, DataHelper(output.get()).GetDataType());
			{
				
				Yap::VariableSpace variable(output->GetVariables());
				int ready_phasesteps = variable.Get<int>(L"ready_phasesteps");
				unsigned int length;
				unsigned int index;
				dynamic_cast<Yap::Dimensions*>(output->GetDimensions())->GetDimensionInfo2(Yap::DimensionSlice, index, length);
				int slice_index = index;
				dynamic_cast<Yap::Dimensions*>(data->GetDimensions())->GetDimensionInfo2(Yap::DimensionChannel, index, length);
				int channel_index = index;

				wstring info = wstring(L"<SliceIterator>") + L"::Feed -----channel_index = " + to_wstring(channel_index)
					+ L"----slice_index = " + to_wstring(channel_index)
					+ L"----ready_phasesteps = " + to_wstring(ready_phasesteps) + L"-------------";
				LOG_TRACE(info.c_str(), L"BasicRecon");
			}
			Feed(L"Output", output.get());
		}

		// output->SetSliceLocalization(GetParams(), i);
	}

	return true;
}
