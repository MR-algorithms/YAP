#include "ModulePhase.h"

#include "Interface/Client/DataHelper.h"

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

}

Yap::ModulePhase::ModulePhase(const ModulePhase& rhs):
	ProcessorImpl(rhs)
{

}

ModulePhase::~ModulePhase()
{
}


bool Yap::ModulePhase::OnInit()
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Module", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);
	AddOutput(L"Phase", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);

	return true;
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
			auto module = YapShared(new DoubleData(data->GetDimensions()));

			GetModule(GetDataArray<complex<double>>(data),
				GetDataArray<double>(module.get()),
				input_data.GetDataSize());

			Feed(L"Module", module.get());
		}
		
		else
		{
			auto module = YapShared(new FloatData(data->GetDimensions()));

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
			auto phase = YapShared(new DoubleData(data->GetDimensions()));

			GetPhase(GetDataArray<complex<double>>(data), GetDataArray<double>(phase.get()),
				input_data.GetDataSize());

			Feed(L"Phase", phase.get());
		}
		else
		{
			auto phase = YapShared(new FloatData(data->GetDimensions()));

			GetPhase(GetDataArray<complex<float>>(data), GetDataArray<float>(phase.get()),
				input_data.GetDataSize());

			Feed(L"Phase", phase.get());
		}
		
	}

	return true;
}

Yap::IProcessor * Yap::ModulePhase::Clone()
{
	return new (nothrow) ModulePhase(*this);
}


