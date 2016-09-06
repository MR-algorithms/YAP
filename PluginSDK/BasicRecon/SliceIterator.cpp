#include "SliceIterator.h"

#include "Interface/Client/DataHelper.h"
#include <complex>
#include "Interface/Implement/DataImpl.h"

using namespace Yap;
using namespace std;

SliceIterator::SliceIterator(void) :
	ProcessorImpl(L"SliceIterator")
{
}

SliceIterator::SliceIterator( const SliceIterator& rhs)
	: ProcessorImpl(rhs)
{

}


SliceIterator::~SliceIterator(void)
{
}


bool Yap::SliceIterator::OnInit()
{
	AddInput(L"Input", 3, DataTypeComplexFloat | DataTypeUnsignedShort);
	AddOutput(L"Output", 2, DataTypeComplexFloat | DataTypeUnsignedShort);

	return true;
}

IProcessor* SliceIterator::Clone()
{
	return new (std::nothrow) SliceIterator(*this);
}

bool SliceIterator::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && ((Yap::GetDataArray<complex<float>>(data) != nullptr) || Yap::GetDataArray<unsigned short>(data) != nullptr));
	assert(Inputs()->Find(name) != nullptr);

	CDataHelper helper(data);

	unsigned int slice_block_size = helper.GetBlockSize(DimensionSlice);

	Dimension slice_dimension = helper.GetDimension(DimensionSlice);
	assert(slice_dimension.type == DimensionSlice);
	
	for (unsigned int i = slice_dimension.start_index; i < slice_dimension.start_index + slice_dimension.length; ++i)
	{
		DimensionsImpl slice_data_dimensions(data->GetDimensions());
		slice_data_dimensions.ModifyDimension(DimensionSlice, 1, i);
		if (data->GetDataType() == DataTypeComplexFloat)
		{
			auto output = YapShared(new CComplexFloatData(
				Yap::GetDataArray<complex<float>>(data) + i * slice_block_size, slice_data_dimensions));
			Feed(L"Output", output.get());
		}
		else
		{
			auto output = YapShared(new CUnsignedShortData(
				Yap::GetDataArray<unsigned short>(data) + i * slice_block_size, slice_data_dimensions));
			Feed(L"Output", output.get());
		}

		// output->SetSliceLocalization(GetParams(), i);		
	}

	return true;
}


