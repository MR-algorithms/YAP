#include "stdafx.h"
#include "CompressedSensing.h"

using namespace std;
using namespace Yap;

CompressedSensing::CompressedSensing():
	ProcessorImpl(L"CompressedSensing")
{
}


CompressedSensing::~CompressedSensing()
{
}

bool Yap::CompressedSensing::OnInit()
{
	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);
	return true;
}

IProcessor * Yap::CompressedSensing::Clone()
{
	return new (nothrow) CompressedSensing(*this);
}

bool Yap::CompressedSensing::Input(const wchar_t * port, IData * data)
{	
	if (wstring(port) == L"Input")
	{

	}
		return true;
}
