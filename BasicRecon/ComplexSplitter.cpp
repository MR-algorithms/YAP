#include "stdafx.h"
#include "ComplexSplitter.h"

using namespace std;

CComplexSplitter::CComplexSplitter()
{
	AddProperty(L"CreatReal", PropertyBool);
	SetBoolProperty(L"CreatReal", 0);
	AddProperty(L"Imaginary", PropertyBool);
	SetBoolProperty(L"Imaginary", 0);

	//one dimension
	AddInputPort(L"Input_D1",1,DataTypeComplexDouble);
	AddOutputPort(L"Real_D1",1,DataTypeDouble);
	AddOutputPort(L"Imaginary_D1", 1,DataTypeDouble);

	//two dimensions
	AddInputPort(L"Input_D2", 2, DataTypeComplexDouble);
	AddOutputPort(L"Real_D2", 2, DataTypeDouble);
	AddOutputPort(L"Imaginary_D2", 2, DataTypeDouble);
}

CComplexSplitter::~CComplexSplitter()
{

}

bool CComplexSplitter::Init()
{
	return false;
}

bool CComplexSplitter::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input__D1" || wstring(port) != L"Input__D2")
		return false;
	
	if (data->GetDataType() != DataTypeComplexDouble)
		return false;

	CDataHelper input_data(data);

	auto * real_data = new Yap::CDoubleData(data->GetDimension());
	auto * imaginary_data = new Yap::CDoubleData(data->GetDimension());

	//one and two dimension(s)
	if (input_data.GetDimensionCount() == 2)
	{	
		Split(reinterpret_cast<std::complex<double> *>(input_data.GetData()),
			reinterpret_cast<double*>(real_data->GetData()),
			reinterpret_cast<double*>(imaginary_data->GetData()),
			input_data.GetHeight()*input_data.GetWidth());

	}
	else if (input_data.GetDimensionCount()==1)
	{
		Split(reinterpret_cast<std::complex<double> *>(input_data.GetData()),
			reinterpret_cast<double*>(real_data->GetData()),
			reinterpret_cast<double*>(imaginary_data->GetData()),
			input_data.GetWidth());
	}
	else 
	{
		return false;
	}

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

