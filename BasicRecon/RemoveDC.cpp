#include "stdafx.h"
#include "RemoveDC.h"
#include <string>
#include "DataHelper.h"
#include "..\Interface\ReconData.h"

using namespace std;
using namespace Yap;

CRemoveDC::CRemoveDC()
{
	AddInputPort(L"Input", 2, DataTypeDouble);
	
	AddProperty(PropertyBool, L"Inplace", L"");
	SetBoolProperty(L"Inplace", true);
	AddProperty(PropertyInt, L"CornerSize", L"");
	SetIntProperty(L"CornerSize", 10);

	AddOutputPort(L"Output", 2, DataTypeDouble);
}


CRemoveDC::~CRemoveDC()
{
}

bool CRemoveDC::Init()
{
	return true;
}

bool CRemoveDC::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;
	if (data->GetDataType() != DataTypeDouble)
		return false;
	CDataHelper input_data(data);
	if (input_data.GetDimensionCount() != 2)
		return false;

	auto * output_data =new Yap::CDoubleData(data->GetDimension());
	unsigned int width = input_data.GetWidth();
	unsigned int height = input_data.GetWidth();
	auto inplace = GetBoolProperty(L"Inplace");
	unsigned int corner_size = GetIntProperty(L"CornerSize");
	RemoveDC(reinterpret_cast<double *>(input_data.GetData()), 
		reinterpret_cast<double *>(output_data->GetData()),
		width, height, inplace, corner_size);

	return true;
}

wchar_t * CRemoveDC::GetId()
{
	return L"RemoveDC";
}
