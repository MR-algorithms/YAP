#include "stdafx.h"
#include "DummyProcessor.h"
#include <string>
#include <iostream>
#include <complex>
#include "..\Interface\ReconData.h"

using namespace std;

CDummyProcessor::CDummyProcessor()
{
	AddInputPort(L"Input", 2, DataTypeFloat);
	AddOutputPort(L"Output", 2, DataTypeFloat);

	AddProperty(L"TestBool", PropertyBool);
	SetBoolProperty(L"TestBool", false);

	AddProperty(L"TestFloat", PropertyFloat);
	SetFloatProperty(L"TestFloat", 1.0);
}


CDummyProcessor::~CDummyProcessor()
{
}

wchar_t * CDummyProcessor::GetId()
{
	return L"Dummy";
}

bool CDummyProcessor::Init()
{
	return true;
}

bool CDummyProcessor::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	Test(data);
// 
// 	Yap::CDimensions dimensions(data->GetDimension());
// 	size_t count = dimensions.TotalDataCount();
// 	wcout << L"Total pixels: " << count << endl;
// 
// 
// 	auto d = reinterpret_cast<double*>(data->GetData());
// 	double sum = 0.0;
// 	for (unsigned int i = 0; i < count; ++i)
// 	{
// 		sum += d[i];
// 	}
// 	wcout << "Sum of all pixels: " << sum << endl;
// 
// 	Feed(L"Output", data);

	return true;
}

bool CDummyProcessor::Test(IData * data)
{
	CDataHelper data_object(data);

	if (data_object.GetDataType() != DataTypeComplexDouble)
		return false;

	if (data_object.GetDimensionCount() != 2)
		return false;

	auto width = data_object.GetWidth();
	auto height = data_object.GetHeight();
	auto data_buffer = reinterpret_cast<complex<double> *> (data_object.GetData());

	return false;
}
