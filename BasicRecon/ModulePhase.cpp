#include "stdafx.h"
#include "ModulePhase.h"
#include "DataHelper.h"
#include "..\Interface\SmartPtr.h"
#include <math.h>
#include <complex>

using namespace Yap;
using namespace std;

CModulePhase::CModulePhase() :
	CProcessorImp(L"ModulePhase")
{
	AddInputPort(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble);
	AddOutputPort(L"Module", YAP_ANY_DIMENSION, DataTypeDouble);
	AddOutputPort(L"Phase", YAP_ANY_DIMENSION, DataTypeDouble);

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

	CDataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble)
		return false;
	if (input_data.GetDimensionCount() != 2)
		return false;

	CDimensionsImp dims;
	dims(DimensionReadout, 0, input_data.GetWidth())
		(DimensionPhaseEncoding, 0, input_data.GetHeight());

	if (is_module)
	{
		auto module = CSmartPtr<CDoubleData>(new CDoubleData(&dims));

		GetModule(reinterpret_cast<complex<double>*>(input_data.GetData()),
			reinterpret_cast<double*>(module->GetData()),
			input_data.GetDataSize());

		Feed(L"Module", module.get());
	}

	if (is_phase)
	{
		auto phase = CSmartPtr<CDoubleData>(new CDoubleData(&dims));

		GetPhase(reinterpret_cast<complex<double>*>(input_data.GetData()), 
			reinterpret_cast<double*>(phase->GetData()),
			input_data.GetDataSize());

		Feed(L"Phase", phase.get());
	}

	return true;
}

bool CModulePhase::GetModule(std::complex<double>* input, 
	double* module,
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

bool CModulePhase::GetPhase(std::complex<double>* input, double* phase,
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
