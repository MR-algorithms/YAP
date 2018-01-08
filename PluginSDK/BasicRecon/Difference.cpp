#include "Difference.h"
#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

using namespace Yap;
using namespace std;

template <typename T>
void calc_difference(T * input_data,
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


Difference::Difference() : ProcessorImpl(L"Difference")
{
	LOG_TRACE(L"Difference constructor called.", L"BasicRecon");
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeAll);
	AddInput(L"Reference", YAP_ANY_DIMENSION, DataTypeAll);

	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeAll);
}

Difference::Difference(const Difference& rhs)
	:ProcessorImpl(rhs),
	_reference_data{rhs._reference_data}
{
	LOG_TRACE(L"Difference copy constructor called.", L"BasicRecon");
}

Difference::~Difference()
{
}

bool Yap::Difference::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) == L"Reference")
	{
		_reference_data = YapShared(data);
		return true;
	}
	else if (wstring(port) == L"Input")
	{
		if (!_reference_data)
			return false;   //need a reference data.

		DataHelper input_data(data);
		DataHelper reference_data(_reference_data.get());

		if (input_data.GetDimensionCount() != reference_data.GetDimensionCount())
			return false;

		if (input_data.GetDataType() != reference_data.GetDataType())
			return false;

		//check every dimension
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
			if (ref_start != input_start)
				return false;
			if (ref_dim_type != input_dim_type)
				return false;
		}

		if (data->GetDataType() == DataTypeFloat)
		{
			auto output_data = CreateData<float>(data);

			calc_difference(GetDataArray<float>(data), GetDataArray<float>(_reference_data.get()),
				GetDataArray<float>(output_data.get()), input_data.GetDataSize());
			return Feed(L"Output", output_data.get());
		}
		else
		{
			auto output_data = CreateData<double>(data);

			calc_difference(GetDataArray<double>(data), GetDataArray<double>(_reference_data.get()),
				GetDataArray<double>(output_data.get()), input_data.GetDataSize());
			return Feed(L"Output", output_data.get());
		}
	}
	else
	{
		return false;
	}
}
