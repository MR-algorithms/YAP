#include "stdafx.h"
#include "SliceIterator.h"

#include "Client/DataHelper.h"
#include <complex>
#include "Implement/DataObject.h"

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
		slice_data_dimensions.SetDimension(DimensionSlice, 1, i);

		if (helper.GetDataType() == DataTypeComplexFloat)
		{
			auto output = YapShared(new ComplexFloatData(
				Yap::GetDataArray<complex<float>>(data) + i * slice_block_size, slice_data_dimensions));

			Feed(L"Output", output.get());
		}
		else
		{
			auto output = YapShared(new UnsignedShortData(
				Yap::GetDataArray<unsigned short>(data) + i * slice_block_size, slice_data_dimensions));

			Feed(L"Output", output.get());
		}

		// output->SetSliceLocalization(GetParams(), i);
	}

	return true;
}


