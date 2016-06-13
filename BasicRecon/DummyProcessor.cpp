#include "stdafx.h"
#include "DummyProcessor.h"
#include <string>
#include <iostream>

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

	CDimensions dimensions(data->GetDimension());
	size_t count = dimensions.TotalDataCount();
	wcout << L"Total pixels: " << count << endl;

	auto d = reinterpret_cast<double*>(data->GetData());
	double sum = 0.0;
	for (unsigned int i = 0; i < count; ++i)
	{
		sum += d[i];
	}
	wcout << "Sum of all pixels: " << sum << endl;

	Feed(L"Output", data);

	return true;
}
