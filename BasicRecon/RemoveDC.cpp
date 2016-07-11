#include "stdafx.h"
#include "RemoveDC.h"
#include <string>
#include "DataHelper.h"
#include "..\Interface\DataImp.h"


using namespace std;
using namespace Yap;

CRemoveDC::CRemoveDC() :
	CProcessorImp(L"RemoveDC")
{
	AddInputPort(L"Input", 2, DataTypeDouble);
	
	AddProperty(PropertyBool, L"Inplace", L"If the processed data will be stored in place.");
	SetBool(L"Inplace", true);
	AddProperty(PropertyInt, L"CornerSize", L"Size of the corners used to estimate noise level.");
	SetInt(L"CornerSize", 10);

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
	auto inplace = GetBool(L"Inplace");
	unsigned int corner_size = GetInt(L"CornerSize");

	auto output_data = CSmartPtr<CDoubleData>(new CDoubleData(data->GetDimensions()));

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

void Yap::CRemoveDC::RemoveDC(double * input_data, 
	double * output_data, 
	size_t width, 
	size_t height, 
	bool inplace, 
	size_t corner_size)
{
	assert(corner_size <= width / 2 && corner_size <= height / 2 && corner_size >= 2);
	double total = 0.0;
	for (unsigned int i = 0; i < corner_size; ++i)
	{
		for (unsigned int j = 0; j < corner_size; ++j)
		{
			total = total +
				*(input_data + width * i + j) +
				*(input_data + width * i + width - j - 1) +
				*(input_data + width * (height - i - 1) + j) +
				*(input_data + width * (height - i - 1) + width - j - 1);
		}
	}
	total /= corner_size * corner_size * 4;

	if (inplace)
	{
		for (unsigned int i = 0; i < width * height; ++i)
		{
			*(input_data + i) -= total;
		}
	}
	else
	{
		for (unsigned int i = 0; i < width * height; ++i)
		{
			*(output_data + i) = *(input_data + i) - total;
		}
	}
}
