#include "stdafx.h"
#include "PhaseCorrector.h"
#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <complex>

using namespace Yap;
using namespace std;

void PhaseCorrect(complex<float> * input,
					float * phase,
					complex<float> * output,
					size_t size)
{
	assert(input != nullptr && phase != nullptr && output != nullptr);

	auto input_end = input + size;
	const complex<float> i(0, 1);
	while (input != input_end)
	{
		*(output++) = *(input++) * exp(-i * (*(phase++)));
	}
}

Yap::PhaseCorrector::PhaseCorrector():
	ProcessorImpl(L"PhaseCorrector")
{
	LOG_TRACE(L"PhaseCorrector constructor called.", L"BasicRecon");
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddInput(L"Phase", YAP_ANY_DIMENSION, DataTypeFloat);

	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexFloat);
}

Yap::PhaseCorrector::PhaseCorrector(const PhaseCorrector& rhs):
	ProcessorImpl(rhs),
	_phase{rhs._phase}
{
	LOG_TRACE(L"PhaseCorrector constructor called.", L"BasicRecon");
}

Yap::PhaseCorrector::~PhaseCorrector()
{
	LOG_TRACE(L"PhaseCorrector destructor called.", L"BasicRecon");
}

bool Yap::PhaseCorrector::Input(const wchar_t * name, IData * data)
{
	if (wstring(name) == L"Phase")
	{
		_phase = YapShared(data);
		return true;
	}
	else if (wstring(name) == L"Input")
	{
		if (!_phase)
		{
			LOG_ERROR(L"<PhaseCorrector> Need a phase data.", L"BasicRecon");
			return false;   //need a phase data.
		}

		DataHelper input_data(data);
		DataHelper phase(_phase.get());

		if (input_data.GetActualDimensionCount() != phase.GetActualDimensionCount())
		{
			LOG_ERROR(L"<PhaseCorrector> Different dimension between phase data and input data.", L"BasicRecon");
			return false;
		}

		if (input_data.GetDataType() != DataTypeComplexFloat || phase.GetDataType() != DataTypeFloat)
		{
			LOG_ERROR(L"<PhaseCorrector> Error input data type!(DataTypeComplexFloat and DataTypeFloat are available)!", L"BasicRecon");
			return false;
		}

		//check every dimension
		auto phase_dims = _phase->GetDimensions();
		auto input_dims = data->GetDimensions();
		DimensionType phase_dim_type, input_dim_type;
		unsigned int phase_start, input_start, phase_length, input_length;

		for (unsigned int i = 0; i < input_data.GetActualDimensionCount(); i++)
		{
			phase_dims->GetDimensionInfo(i, phase_dim_type, phase_start, phase_length);
			input_dims->GetDimensionInfo(i, input_dim_type, input_start, input_length);

			if (phase_length != input_length)
			{
				LOG_ERROR(L"<PhaseCorrector> Input data length not match with phase data length!", L"BasicRecon");
				return false;
			}
			if (phase_start != input_start)
			{
				LOG_ERROR(L"<PhaseCorrector> Input data start not match with phase data start!", L"BasicRecon");
				return false;
			}
			if (phase_dim_type != input_dim_type)
			{
				LOG_ERROR(L"<PhaseCorrector> Input data dimension type not match with phase data dimension type!", L"BasicRecon");
				return false;
			}
		}

		auto output = CreateData<complex<float>>(data);
		PhaseCorrect(GetDataArray<complex<float>>(data), 
					 GetDataArray<float>(_phase.get()),
					 GetDataArray<complex<float>>(output.get()), 
					 input_data.GetDataSize());
		_phase.reset();
		return Feed(L"Output", output.get());
	}
}
