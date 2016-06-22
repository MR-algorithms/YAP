#include "stdafx.h"
#include "ModulePhase.h"
#include "DataHelper.h"
#include "..\Interface\SmartPtr.h"
#include <math.h>
#include <complex>

using namespace Yap;
using namespace std;

CModulePhase::CModulePhase()
{
	AddInputPort(L"Input", 2, DataTypeComplexDouble);
	AddOutputPort(L"Module", 2, DataTypeDouble);
	AddOutputPort(L"Phase", 2, DataTypeDouble);

	AddProperty(L"CreateModeule", PropertyBool);
	AddProperty(L"CreatePhase", PropertyBool);
}


CModulePhase::~CModulePhase()
{
}

bool CModulePhase::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	auto is_module = GetBoolProperty(L"CreateModule");
	auto is_phase = GetBoolProperty(L"CreatePhase");

	CDataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble)
		return false;
	if (input_data.GetDimensionCount() != 2)
		return false;

	CDimensions dims;
	dims(DimensionReadout, 0, input_data.GetWidth())
		(DimensionPhaseEncoding, 0, input_data.GetHeight());

	if (is_module)
	{
		auto module = CSmartPtr<CDoubleData>(new CDoubleData(&dims));

		GetModule(reinterpret_cast<complex<double>*>(input_data.GetData()),
			reinterpret_cast<double*>(module->GetData()),
			input_data.GetWidth(), input_data.GetHeight());

		Feed(L"Module", module.get());
	}

	if (is_phase)
	{
		auto phase = CSmartPtr<CDoubleData>(new CDoubleData(&dims));

		GetPhase(reinterpret_cast<complex<double>*>(input_data.GetData()), 
			reinterpret_cast<double*>(phase->GetData()),
			input_data.GetWidth(), input_data.GetHeight());

		Feed(L"Phase", phase.get());
	}

	return true;
}

wchar_t * CModulePhase::GetId()
{
	return L"ModulePhase";
}

bool CModulePhase::GetModule(std::complex<double>* input, double* module,
	unsigned int width, unsigned int height)
{
	assert(input != nullptr && module != nullptr);

	for (unsigned int pixel = 0; pixel < width * height; ++pixel)
	{
		*(module + pixel) = abs(*(input + pixel));
	}

	return true;
}

bool CModulePhase::GetPhase(std::complex<double>* input, double* phase,
	unsigned int width, unsigned int height)
{
	assert(input != nullptr && phase != nullptr);

	for (unsigned int pixel = 0; pixel < width * height; ++pixel)
	{
		*(phase + pixel) = arg(*(input + pixel));
	}

	return true;
}
