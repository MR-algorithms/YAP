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

CModulePhase::CModulePhase() :
	ProcessorImpl(L"ModulePhase")
{
	AddInputPort(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutputPort(L"Module", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);
	AddOutputPort(L"Phase", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);

}

CModulePhase::~CModulePhase()
{
}

bool CModulePhase::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	if (data->GetDataType() != DataTypeComplexDouble && data->GetDataType() != DataTypeComplexFloat)
		return false;

	CDataHelper input_data(data);

	auto want_module = OutportLinked(L"Module");
	auto want_phase = OutportLinked(L"Phase");

	if (input_data.GetActualDimensionCount() != 2)
		return false;

	CDimensionsImpl dims;
	dims(DimensionReadout, 0, input_data.GetWidth())
		(DimensionPhaseEncoding, 0, input_data.GetHeight());

	if (want_module)
	{
		if (data->GetDataType() == DataTypeComplexDouble)
		{
			auto module = YapSharedObject(new CDoubleData(&dims));

			GetModule(reinterpret_cast<complex<double>*>(input_data.GetData()),
				reinterpret_cast<double*>(module->GetData()),
				input_data.GetDataSize());

			Feed(L"Module", module.get());
		}
		
		else
		{
			auto module = YapSharedObject(new CFloatData(&dims));

			GetModule(reinterpret_cast<complex<float>*>(input_data.GetData()),
				reinterpret_cast<float*>(module->GetData()),
				input_data.GetDataSize());

			Feed(L"Module", module.get());
		}

	}

	if (want_phase)
	{
		if (data->GetDataType() == DataTypeComplexDouble)
		{
			auto phase = YapSharedObject(new CDoubleData(&dims));

			GetPhase(reinterpret_cast<complex<double>*>(input_data.GetData()),
				reinterpret_cast<double*>(phase->GetData()),
				input_data.GetDataSize());

			Feed(L"Phase", phase.get());
		}
		else
		{
			auto phase = YapSharedObject(new CFloatData(&dims));

			GetPhase(reinterpret_cast<complex<float>*>(input_data.GetData()),
				reinterpret_cast<float*>(phase->GetData()),
				input_data.GetDataSize());

			Feed(L"Phase", phase.get());
		}
		
	}

	return true;
}

Yap::IProcessor * Yap::CModulePhase::Clone()
{
	try
	{
		return new CModulePhase;
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

