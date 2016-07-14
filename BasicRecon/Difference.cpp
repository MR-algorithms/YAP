#include "stdafx.h"
#include "Difference.h"
#include "DataHelper.h"

using namespace Yap;
using namespace std;

template <typename T>
void Difference(T * input_data,
	T * reference_data,
	T * output_data,
	size_t size)
{
	assert(input_data != nullptr && reference_data != nullptr && output_data != nullptr);

	auto input_end = input_data + size;
	while (input_data != input_end)
	{
		*(output_data++) = *(reference_data++) - *(input_data++);
	}
}


CDifference::CDifference() : CProcessorImp(L"Difference"),
	_reference_data(nullptr)
{
	AddInputPort(L"Input", YAP_ANY_DIMENSION, DataTypeAll);
	AddInputPort(L"Reference", YAP_ANY_DIMENSION, DataTypeAll);

	AddOutputPort(L"Output", YAP_ANY_DIMENSION, DataTypeAll);
}


CDifference::~CDifference()
{
}

bool Yap::CDifference::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) == L"Reference")
	{
		_reference_data = data;
	}
	else if (wstring(port) == L"Input")
	{
		if (_reference_data == nullptr)
			return false;   //need a reference data.

		CDataHelper input_data(data);
		CDataHelper reference_data(_reference_data);

		if (input_data.GetDimensionCount() != reference_data.GetDimensionCount())
			return false;

		if (input_data.GetDataType() != reference_data.GetDataType())
			return false;

		//check every dimension size
		auto ref_dim = _reference_data->GetDimensions();
		auto input_dim = data->GetDimensions();
		DimensionType ref_dim_type, input_dim_type;
		unsigned int ref_start, input_start, ref_length, input_length;

		for (unsigned int i = 0; i < input_data.GetDimensionCount(); i++)
		{
			ref_dim->GetDimensionInfo(i, ref_dim_type, ref_start, ref_length);
			input_dim->GetDimensionInfo(i, input_dim_type, input_start, input_length);

			if (ref_length != input_length)
				return false;
		}

		//more data type?
		if (data->GetDataType() == DataTypeFloat)
		{
			auto output_data = CSmartPtr<CFloatData>(new CFloatData(data->GetDimensions()));

			Difference(reinterpret_cast<float*>(input_data.GetData()),
				reinterpret_cast<float*>(reference_data.GetData()),
				reinterpret_cast<float*>(output_data->GetData()),
				input_data.GetDataSize());
			Feed(L"Output", output_data.get());
		}
		else
		{
			auto output_data = CSmartPtr<CDoubleData>(new CDoubleData(data->GetDimensions()));

			Difference(reinterpret_cast<double*>(input_data.GetData()),
				reinterpret_cast<double*>(reference_data.GetData()),
				reinterpret_cast<double*>(output_data->GetData()),
				input_data.GetDataSize());
			Feed(L"Output", output_data.get());
		}
	}

	return true;
}