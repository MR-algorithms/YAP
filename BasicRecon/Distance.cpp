#include "stdafx.h"
#include "Distance.h"

using namespace Yap;

CDistance::CDistance() : CProcessorImp(L"Distance")
{
	AddInputPort(L"Start", 2, DataTypeComplexDouble);
	AddInputPort(L"Destination", 2, DataTypeComplexDouble);
	AddOutputPort(L"Output", 2, DataTypeComplexDouble);

	AddProperty(PropertyInt, L"Exponent", L"Exponent number of distance.");

}


CDistance::~CDistance()
{
}

bool Yap::CDistance::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Destination")
		return false;

}
