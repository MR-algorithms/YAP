#include "ModulePhase.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <math.h>
#include <complex>

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
	LOG_TRACE(L"ModulePhase constructor called.", L"BasicRecon");
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Module", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);
	AddOutput(L"Phase", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);
}

Yap::ModulePhase::ModulePhase(const ModulePhase& rhs):
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"ModulePhase constructor called.", L"BasicRecon");
}

ModulePhase::~ModulePhase()
{
	LOG_TRACE(L"ModulePhase destructor called.", L"BasicRecon");
}

bool ModulePhase::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	if (data->GetDataType() != DataTypeComplexDouble && data->GetDataType() != DataTypeComplexFloat)
		return false;

	DataHelper input_data(data);

	auto want_module = OutportLinked(L"Module");
	auto want_phase = OutportLinked(L"Phase");

	if (want_module)
	{
		if (data->GetDataType() == DataTypeComplexDouble)
		{
			auto module = CreateData<double>(data->GetDimensions());

			GetModule(GetDataArray<complex<double>>(data),
				GetDataArray<double>(module.get()),
				input_data.GetDataSize());

			Feed(L"Module", module.get());
		}
		
		else
		{
			auto module = CreateData<float>(data->GetDimensions());

			GetModule(GetDataArray<complex<float>>(data),
				GetDataArray<float>(module.get()),
				input_data.GetDataSize());

			Feed(L"Module", module.get());
		}
	}

	if (want_phase)
	{
		if (data->GetDataType() == DataTypeComplexDouble)
		{
			auto phase = CreateData<double>(data->GetDimensions());

			GetPhase(GetDataArray<complex<double>>(data), GetDataArray<double>(phase.get()),
				input_data.GetDataSize());

			Feed(L"Phase", phase.get());
		}
		else
		{
			auto phase = CreateData<float>(data->GetDimensions());

			GetPhase(GetDataArray<complex<float>>(data), GetDataArray<float>(phase.get()),
				input_data.GetDataSize());

			Feed(L"Phase", phase.get());
		}
	}

	return true;
}
