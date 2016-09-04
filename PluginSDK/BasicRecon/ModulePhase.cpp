#include "ModulePhase.h"

#include "../../Shared/Interface/Client/DataHelper.h"

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
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Module", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);
	AddOutput(L"Phase", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);

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
			auto module = YapShared(new CDoubleData(&dims));

			GetModule(GetDataArray<complex<double>>(data),
				GetDataArray<double>(module.get()),
				input_data.GetDataSize());

			Feed(L"Module", module.get());
		}
		
		else
		{
			auto module = YapShared(new CFloatData(&dims));

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
			auto phase = YapShared(new CDoubleData(&dims));

			GetPhase(GetDataArray<complex<double>>(data), GetDataArray<double>(phase.get()),
				input_data.GetDataSize());

			Feed(L"Phase", phase.get());
		}
		else
		{
			auto phase = YapShared(new CFloatData(&dims));

			GetPhase(GetDataArray<complex<float>>(data), GetDataArray<float>(phase.get()),
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

