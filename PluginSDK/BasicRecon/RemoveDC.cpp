#include "RemoveDC.h"

#include "Yap/DataHelper.h"
#include "Interface/DataImp.h"

#include <string>

using namespace std;
using namespace Yap;

template <typename T>
void RemoveDC(T * input_data,
	T * output_data,
	size_t width,
	size_t height,
	bool inplace,
	size_t corner_size)
{
	assert(corner_size <= width / 2 && corner_size <= height / 2 && corner_size >= 2);
	T total = 0.0;

	T * p1 = input_data;
	T * p2 = input_data + width - corner_size;
	T * p3 = input_data + width * (height - corner_size);
	T * p4 = input_data + width * (height - corner_size) + width - corner_size;

	for (unsigned int i = 0; i < corner_size; ++i)
	{
		for (unsigned int j = 0; j < corner_size; ++j)
		{
			total += (*p1++) + (*p2++) + (*p3++) + (*p4++);
		}
		p1 += (width - corner_size);
		p2 += (width - corner_size);
		p3 += (width - corner_size);
		p4 += (width - corner_size);
	}

	total /= static_cast<T::value_type>(corner_size * corner_size) * 4;

	auto end = input_data + width * height;

	if (inplace)
	{
		for (; input_data != end; )
		{
			*input_data++ -= total;
		}
	}
	else
	{
		for (; input_data != end;)
		{
			*output_data++ = *input_data++ - total;
		}
	}
}

CRemoveDC::CRemoveDC() :
	CProcessorImp(L"RemoveDC")
{
	AddInputPort(L"Input", 2, DataTypeComplexDouble | DataTypeComplexFloat);
	
	AddProperty(PropertyBool, L"Inplace", L"If the processed data will be stored in place.");
	SetBool(L"Inplace", true);
	AddProperty(PropertyInt, L"CornerSize", L"Size of the corners used to estimate noise level.");
	SetInt(L"CornerSize", 10);

	AddOutputPort(L"Output", 2, DataTypeComplexDouble | DataTypeComplexFloat);
}


CRemoveDC::~CRemoveDC()
{
}

bool CRemoveDC::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	if (data->GetDataType() != DataTypeComplexDouble && data->GetDataType() != DataTypeComplexFloat)
		return false;

	CDataHelper input_data(data);
	if (input_data.GetActualDimensionCount() != 2)
		return false;

	unsigned int width = input_data.GetWidth();
	unsigned int height = input_data.GetHeight();
	auto inplace = GetBool(L"Inplace");
	unsigned int corner_size = GetInt(L"CornerSize");
	if (corner_size >= height / 2 || corner_size >= width / 2 || corner_size < 2)
		return false;

	if (data->GetDataType() == DataTypeComplexFloat)
	{
		if (inplace)
		{
			RemoveDC<complex<float>>(reinterpret_cast<complex<float>*>(input_data.GetData()),
				nullptr, width, height, inplace, corner_size);
			Feed(L"Output", data);
		}
		else
		{
			auto output_data = CSmartPtr<CComplexFloatData>(new CComplexFloatData(data->GetDimensions()));

			RemoveDC(reinterpret_cast<complex<float>*>(input_data.GetData()),
				reinterpret_cast<complex<float>*>(output_data->GetData()),
				width, height, inplace, corner_size);
			Feed(L"Output", output_data.get());
		}

	}
	else // DataTypeComplexDouble
	{
		if (inplace)
		{
			RemoveDC<complex<double>>(reinterpret_cast<complex<double>*>(input_data.GetData()),
				nullptr, width, height, inplace, corner_size);
			Feed(L"Output", data);
		}
		else
		{
			auto output_data = CSmartPtr<CComplexDoubleData>(new CComplexDoubleData(data->GetDimensions()));

			RemoveDC(reinterpret_cast<complex<double>*>(input_data.GetData()),
				reinterpret_cast<complex<double>*>(output_data->GetData()),
				width, height, inplace, corner_size);
			Feed(L"Output", output_data.get());
		}
	}

	return true;
}

Yap::IProcessor * Yap::CRemoveDC::Clone()
{
	try
	{
		return new CRemoveDC;
	}
	catch (bad_alloc&)
	{
		return nullptr;
	}
}
