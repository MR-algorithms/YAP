#include "stdafx.h"
#include "ComplexSplitter.h"

using namespace std;
using namespace Yap;

CComplexSplitter::CComplexSplitter() :
	CProcessorImp(L"ComplexSplitter")
{
	AddProperty(PropertyBool, L"CreatReal", L"Create Real Image");
	SetBoolProperty(L"CreatReal", false);
	AddProperty(PropertyBool, L"Imaginary", L"Create Imaginary image");
	SetBoolProperty(L"Imaginary", false);

	//ANY  DIMENSION
	AddInputPort(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble);
	AddOutputPort(L"Real", YAP_ANY_DIMENSION, DataTypeDouble);
	AddOutputPort(L"Imaginary", YAP_ANY_DIMENSION, DataTypeDouble);
// 
// 	//two dimensions
// 	AddInputPort(L"Input_D2", 2, DataTypeComplexDouble);
// 	AddOutputPort(L"Real_D2", 2, DataTypeDouble);
// 	AddOutputPort(L"Imaginary_D2", 2, DataTypeDouble);
// 
// 	//three dimensions
// 	AddInputPort(L"Input_D3",3, DataTypeComplexDouble);
// 	AddOutputPort(L"Real_D3",3,DataTypeDouble);
// 	AddOutputPort(L"Imaginary_D3",3, DataTypeDouble);

}

CComplexSplitter::~CComplexSplitter()
{

}

bool CComplexSplitter::Init()
{
	return true;
}

bool CComplexSplitter::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input" )
		return false;
	
	if (data->GetDataType() != DataTypeComplexDouble)
		return false;

	CDataHelper input_data(data);

	auto * real_data = new Yap::CDoubleData(data->GetDimension());
	auto * imaginary_data = new Yap::CDoubleData(data->GetDimension());

	unsigned int size = -1;
	//one, two and three dimension(s), ANY¡¡DIMENSION
	size = input_data.GetDataSize();

	Split(reinterpret_cast<std::complex<double> *>(input_data.GetData()),
		reinterpret_cast<double*>(real_data->GetData()),
		reinterpret_cast<double*>(imaginary_data->GetData()),
		size);

	if (GetBoolProperty(L"CreatReal"))
	{
		Feed(L"Real", real_data);
	}
	if (GetBoolProperty(L"Imaginary"))
	{
		Feed(L"Imaginary", imaginary_data);
	}

	return true;

}

