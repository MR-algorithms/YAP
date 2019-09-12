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
		slice_data_dimensions.SetDimension(DimensionSlice, 1, i);
		//Add variable "slice_index" to the variable space.
		AddSliceindexParam(data, i);

		//VariableSpace variables(data->GetVariables());
		//variables.AddVariable(L"int", L"slice_index", L"slice index.");
		//variables.Set(L"slice_index", static_cast<int>(i) );
		//

		if (helper.GetDataType() == DataTypeComplexFloat)
		{
			auto output = CreateData<complex<float>>(data,
				Yap::GetDataArray<complex<float>>(data) + i * slice_block_size, slice_data_dimensions, data);

			Feed(L"Output", output.get());
		}
		else
		{
			auto output = CreateData<unsigned short>(data,
				Yap::GetDataArray<unsigned short>(data) + i * slice_block_size, slice_data_dimensions, data);

			Feed(L"Output", output.get());
		}

		// output->SetSliceLocalization(GetParams(), i);
	}

	return true;
}
/**
	\remark Add "Slice_Index" param to IData in pipeline. 
*/
bool SliceIterator::AddSliceindexParam(IData *data, int index) const
{
	DataHelper helper(data);

	if (nullptr == data->GetVariables())
	{
		VariableSpace variable;
		//
		if (helper.GetDataType() == DataTypeComplexFloat)
		{
			dynamic_cast<Yap::DataObject<std::complex<float>>*>(data)->SetVariables(variable.Variables());
		}
		else
		{
			dynamic_cast<Yap::DataObject<unsigned short>*>(data)->SetVariables(variable.Variables());
		}
		

	}

	VariableSpace variables(data->GetVariables());

	variables.AddVariable(L"int", L"slice_index", L"slice index.");
	variables.Set(L"slice_index", static_cast<int>(index));
	return true;

}

