#include "stdafx.h"
#include "ZeroFilling.h"
#include <string>
#include <complex>

#include "..\Interface\SmartPtr.h"
#include "DataHelper.h"
#include "..\Interface\ReconData.h"

using namespace std;
using namespace Yap;

CZeroFilling::CZeroFilling()
{
	AddInputPort(L"Input", 2, DataTypeComplexDouble);
	AddOutputPort(L"Output", 2, DataTypeComplexDouble);

	AddProperty(L"DestWidth", PropertyInt);
	AddProperty(L"DestHeight", PropertyInt);
	AddProperty(L"Left", PropertyInt);
	AddProperty(L"Top", PropertyInt);
}


CZeroFilling::~CZeroFilling()
{
}

bool CZeroFilling::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	unsigned int dest_width = GetIntProperty(L"DestWidth");
	unsigned int dest_height = GetIntProperty(L"DestHeight");

	CDataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble)
		return false;
	if (input_data.GetDimensionCount() != 2)
		return false;

	if (dest_width < input_data.GetWidth() || dest_height < input_data.GetHeight())
		return false;

	Yap::CDimensions dims;
	dims(DimensionReadout, 0, dest_width)
		(DimensionPhaseEncoding, 0, dest_height);

	auto output = CSmartPtr<CComplexDoubleData>(new CComplexDoubleData(&dims));
	ZeroFilling(reinterpret_cast<complex<double>*>(output->GetData()), dest_width, dest_height,
		reinterpret_cast<complex<double>*>(input_data.GetData()), input_data.GetWidth(), input_data.GetHeight());

	Feed(L"Output", output.get());

	return true;
}

wchar_t * CZeroFilling::GetId()
{
	return L"ZeroFilling";
}

bool CZeroFilling::ZeroFilling(complex<double>* dest, unsigned int dest_width, unsigned int dest_height,
	complex<double>* source, unsigned int source_width, unsigned int source_height)
{
	assert(dest != nullptr && source != nullptr);
	assert(dest_width >= source_width && dest_height >= source_height);

	memset(dest, 0, dest_width * dest_height * sizeof(complex<double>));
	for (unsigned int row = 0; row < source_height; ++row)
	{
		memcpy(dest + (dest_height - source_height) / 2 * dest_width + (dest_width - source_width) / 2, 
			source + row * source_width, source_width * sizeof(complex<double>));
	}

	return true;
}
