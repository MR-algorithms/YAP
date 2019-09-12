#include "ModulePhase.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <math.h>
#include <complex>
#include <algorithm>

using namespace Yap;
using namespace std;

template <typename T>
bool GetModule(complex<T>* input,
	T* module,
	size_t size)
{
	assert(input != nullptr && module != nullptr);

	auto input_end = input + size;
	while (input != input_end)
	{
		*(module++) = abs(*(input++));
	}

	return true;
}

template <typename T>
bool GetPhase(complex<T>* input, T* phase,
	size_t size)
{
	assert(input != nullptr && phase != nullptr);

	auto input_end = input + size;
	while (input != input_end)
	{
		*(phase++) = arg(*(input++));
	}

	return true;
}

ModulePhase::ModulePhase(void) :
	ProcessorImpl(L"ModulePhase")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Module", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);
	AddOutput(L"Phase", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);
}

Yap::ModulePhase::ModulePhase(const ModulePhase& rhs):
	ProcessorImpl(rhs)
{
}

ModulePhase::~ModulePhase()
{
}

bool ModulePhase::Input(const wchar_t * port, IData * data)
{
	// Do some check.
	if (data == nullptr)
	{
		LOG_ERROR(L"<ModulePhase> Invalid input data!", L"BasicRecon");
		return false;
	}
	if (_wcsicmp(port, L"Input") != 0)
	{
		LOG_ERROR(L"<ModulePhase> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexFloat && input_data.GetDataType() != DataTypeComplexDouble)
	{
		LOG_ERROR(L"<ModulePhase> Error input data type!(DataTypeComplexFloat and DataTypeComplexDouble available)!", L"BasicRecon");
		return false;
	}

	auto want_module = OutportLinked(L"Module");
	auto want_phase = OutportLinked(L"Phase");

	if (want_module)
	{
		if (data->GetDataType() == DataTypeComplexDouble)
		{
			auto module = CreateData<double>(data);

			GetModule(GetDataArray<complex<double>>(data),
				GetDataArray<double>(module.get()),
				input_data.GetDataSize());

			return Feed(L"Module", module.get());
		}
		
		else
		{
			auto module = CreateData<float>(data);

			GetModule(GetDataArray<complex<float>>(data),
				GetDataArray<float>(module.get()),
				input_data.GetDataSize());

			TestChannelOnSlice(module.get());
			return Feed(L"Module", module.get());
		}
	}

	if (want_phase)
	{
		if (data->GetDataType() == DataTypeComplexDouble)
		{
			auto phase = CreateData<double>(data);

			GetPhase(GetDataArray<complex<double>>(data), GetDataArray<double>(phase.get()),
				input_data.GetDataSize());

			return Feed(L"Phase", phase.get());
		}
		else
		{
			auto phase = CreateData<float>(data);

			GetPhase(GetDataArray<complex<float>>(data), GetDataArray<float>(phase.get()),
				input_data.GetDataSize());

			return Feed(L"Phase", phase.get());
		}
	}

	return true;
}


void ModulePhase::TestChannelOnSlice(IData *output)
{
	auto temp = output->GetDimensions();
	Dimensions data_dimentions(output->GetDimensions());
	DataHelper helper(output);
	Dimension channel_dimention = helper.GetDimension(DimensionChannel);
	Dimension read_dimention = helper.GetDimension(DimensionReadout);
	Dimension phase_dimention = helper.GetDimension(DimensionPhaseEncoding);


	assert(channel_dimention.length == 1);

	unsigned int width = read_dimention.length;
	unsigned int height = phase_dimention.length;

	float* p = GetDataArray<float>(output);
	assert(channel_dimention.length == 1);
	float max = *std::max_element<float*>(p, p + width * height);
	float max2 = *std::max_element<float*>(p, p + 5);
	float* tempp = p + channel_dimention.start_index * 20 * width;
	for (unsigned int i = 0; i < 5*width; i++)
	{
		
		*tempp = max;
		++tempp;
	}

	return;

}