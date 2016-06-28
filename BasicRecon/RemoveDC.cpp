#include "stdafx.h"
#include "RemoveDC.h"
#include <string>
#include "DataHelper.h"
#include "..\Interface\ReconData.h"


using namespace std;
using namespace Yap;

CRemoveDC::CRemoveDC() :
	CProcessorImp(L"RemoveDC")
{
	AddInputPort(L"Input", 2, DataTypeDouble);
	
	AddProperty(PropertyBool, L"Inplace", L"The remove DC is in the source's place.");
	SetBoolProperty(L"Inplace", true);
	AddProperty(PropertyInt, L"CornerSize", L"corner patch size of remove DC.");
	SetIntProperty(L"CornerSize", 10);

	AddOutputPort(L"Output", 2, DataTypeDouble);
}


CRemoveDC::~CRemoveDC()
{
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

	unsigned int width = input_data.GetWidth();
	unsigned int height = input_data.GetHeight();
	auto inplace = GetBoolProperty(L"Inplace");
	unsigned int corner_size = GetIntProperty(L"CornerSize");

	auto output_data = CSmartPtr<CDoubleData>(new CDoubleData(data->GetDimension()));

	if (corner_size >= height / 2 || corner_size >= width / 2 || corner_size < 2)
		return false;

	RemoveDC(reinterpret_cast<double *>(input_data.GetData()), 
		reinterpret_cast<double*>(output_data->GetData()),
		width, height, inplace, corner_size);

	if (inplace)
	{
		Feed(L"Output", data);
	} 
	else
	{
		Feed(L"Output", output_data.get());
	}

	return true;
}
