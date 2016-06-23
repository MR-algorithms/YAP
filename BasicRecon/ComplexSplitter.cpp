#include "stdafx.h"
#include "ComplexSplitter.h"

using namespace std;
using namespace Yap;

CComplexSplitter::CComplexSplitter()
{
	AddProperty(PropertyBool, L"CreatReal", L"");
	SetBoolProperty(L"CreatReal", false);
	AddProperty(PropertyBool, L"Imaginary", L"");
	SetBoolProperty(L"Imaginary", false);

	//one dimension
	AddInputPort(L"Input_D1",1,DataTypeComplexDouble);
	AddOutputPort(L"Real_D1",1,DataTypeDouble);
	AddOutputPort(L"Imaginary_D1", 1,DataTypeDouble);

	//two dimensions
	AddInputPort(L"Input_D2", 2, DataTypeComplexDouble);
	AddOutputPort(L"Real_D2", 2, DataTypeDouble);
	AddOutputPort(L"Imaginary_D2", 2, DataTypeDouble);

	//three dimensions
	AddInputPort(L"Input_D3",3, DataTypeComplexDouble);
	AddOutputPort(L"Real_D3",3,DataTypeDouble);
	AddOutputPort(L"Imaginary_D3",3, DataTypeDouble);

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
	if (wstring(port) != L"Input__D1" || wstring(port) != L"Input__D2" || wstring(port) != L"Input_D3")
		return false;
	
	if (data->GetDataType() != DataTypeComplexDouble)
		return false;

	CDataHelper input_data(data);

	auto * real_data = new Yap::CDoubleData(data->GetDimension());
	auto * imaginary_data = new Yap::CDoubleData(data->GetDimension());

	unsigned int size = -1;
	//one, two and three dimension(s)
	if (input_data.GetDimensionCount() == 2)
	{	
			size = input_data.GetHeight()*input_data.GetWidth();
	}
	else if (input_data.GetDimensionCount()==1)
	{
			size = input_data.GetWidth();
	}
	else if (input_data.GetDimensionCount() == 3)
	{

		size = input_data.GetWidth()* input_data.GetHeight() * input_data.GetSlice();
	}
	else
	{
		return false;
	}

	Split(reinterpret_cast<std::complex<double> *>(input_data.GetData()),
		reinterpret_cast<double*>(real_data->GetData()),
		reinterpret_cast<double*>(imaginary_data->GetData()),
		size);

	if (GetBoolProperty(L"CreatReal"))
	{
		if (input_data.GetDimensionCount() == 1)
			Feed(L"Real_D1", real_data);
		if (input_data.GetDimensionCount() == 2)
			Feed(L"Real_D2", real_data);
	}
	if (GetBoolProperty(L"Imaginary"))
	{
		if (input_data.GetDimensionCount() == 1)
			Feed(L"Imaginary_D1", imaginary_data);
		if (input_data.GetDimensionCount() == 2)
			Feed(L"Imaginary_D2", imaginary_data);
	}

	return true;

}

wchar_t * CComplexSplitter::GetId()
{
	return L"ComplexSplitter";
}

