#include "stdafx.h"
#include "ModulePhase.h"
#include "DataHelper.h"
#include "..\Interface\SmartPtr.h"
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
	CProcessorImp(L"ModulePhase")
{
	AddInputPort(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutputPort(L"Module", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);
	AddOutputPort(L"Phase", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);

	AddProperty(PropertyBool, L"CreateModule", L"If true, the processor will output Modules of the input data.");
	SetBool(L"CreateModule", true);

	AddProperty(PropertyBool, L"CreatePhase", L"If true, the processor will output Phases of the input data.");
	SetBool(L"CreatePhase", false);
}

CModulePhase::~CModulePhase()
{
}

bool CModulePhase::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	auto is_module = GetBool(L"CreateModule");
	auto is_phase = GetBool(L"CreatePhase");

	if (data->GetDataType() != DataTypeComplexDouble && data->GetDataType() != DataTypeComplexFloat)
		return false;
	CDataHelper input_data(data);
	if (input_data.GetDimensionCount() != 2)
		return false;

	CDimensionsImp dims;
	dims(DimensionReadout, 0, input_data.GetWidth())
		(DimensionPhaseEncoding, 0, input_data.GetHeight());

	if (is_module)
	{
		if (data->GetDataType() == DataTypeComplexDouble)
		{
			auto module = CSmartPtr<CDoubleData>(new CDoubleData(&dims));

			GetModule(reinterpret_cast<complex<double>*>(input_data.GetData()),
				reinterpret_cast<double*>(module->GetData()),
				input_data.GetDataSize());

			Feed(L"Module", module.get());
		}
		
		else
		{
			auto module = CSmartPtr<CFloatData>(new CFloatData(&dims));

			GetModule(reinterpret_cast<complex<float>*>(input_data.GetData()),
				reinterpret_cast<float*>(module->GetData()),
				input_data.GetDataSize());

			Feed(L"Module", module.get());
		}

	}

	if (is_phase)
	{
		if (data->GetDataType() == DataTypeComplexDouble)
		{
			auto phase = CSmartPtr<CDoubleData>(new CDoubleData(&dims));

			GetPhase(reinterpret_cast<complex<double>*>(input_data.GetData()),
				reinterpret_cast<double*>(phase->GetData()),
				input_data.GetDataSize());

			Feed(L"Phase", phase.get());
		}
		else
		{
			auto phase = CSmartPtr<CFloatData>(new CFloatData(&dims));

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

