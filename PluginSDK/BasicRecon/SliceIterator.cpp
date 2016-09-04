#include "SliceIterator.h"

#include "../../Shared/Interface/Client/DataHelper.h"
#include <complex>
#include "../../Shared/Interface/Implement/DataImpl.h"

using namespace Yap;
using namespace std;

CSliceIterator::CSliceIterator(void) :
	ProcessorImpl(L"SliceIterator")
{
	AddInputPort(L"Input", 3, DataTypeComplexFloat);
	AddOutputPort(L"Output", 2, DataTypeComplexFloat);
}

CSliceIterator::CSliceIterator( const CSliceIterator& rhs)
	: ProcessorImpl(rhs)
{

}


CSliceIterator::~CSliceIterator(void)
{
}

IProcessor* CSliceIterator::Clone()
{
	try
	{
		auto processor = new CSliceIterator(*this);
		return processor;
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

bool CSliceIterator::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && Yap::GetDataArray<complex<float>>(data) != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	CDataHelper helper(data);

	unsigned int slice_block_size = helper.GetBlockSize(DimensionSlice);

	Dimension slice_dimension = helper.GetDimension(DimensionSlice);
	assert(slice_dimension.type == DimensionSlice);
	
	for (unsigned int i = slice_dimension.start_index; i < slice_dimension.start_index + slice_dimension.length; ++i)
	{
		CDimensionsImpl slice_data_dimensions(data->GetDimensions());
		slice_data_dimensions.ModifyDimension(DimensionSlice, 1, i);

		auto output = YapShared(new CComplexFloatData (
			Yap::GetDataArray<complex<float>>(data) + i * slice_block_size, slice_data_dimensions));

		// output->SetSliceLocalization(GetParams(), i);

		Feed(L"Output", output.get());
	}

	return true;
}


