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
	}
	else if (wstring(name) == L"Input")
	{
		if (!_phase)
			return false;   //need a phase data.

		DataHelper input_data(data);
		DataHelper phase(_phase.get());

		if (input_data.GetActualDimensionCount() != phase.GetActualDimensionCount())
			return false;

		if (input_data.GetDataType() != DataTypeComplexFloat ||
			phase.GetDataType() != DataTypeFloat)
			return false;

		//check every dimension
		auto phase_dims = _phase->GetDimensions();
		auto input_dims = data->GetDimensions();
		DimensionType phase_dim_type, input_dim_type;
		unsigned int phase_start, input_start, phase_length, input_length;

		for (unsigned int i = 0; i < input_data.GetDimensionCount(); i++)
		{
			phase_dims->GetDimensionInfo(i, phase_dim_type, phase_start, phase_length);
			input_dims->GetDimensionInfo(i, input_dim_type, input_start, input_length);

			if (phase_length != input_length)
				return false;
			if (phase_start != input_start)
				return false;
			if (phase_dim_type != input_dim_type)
				return false;
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
