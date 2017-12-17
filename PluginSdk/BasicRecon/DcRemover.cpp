#include "DcRemover.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

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

DcRemover::DcRemover() :
	ProcessorImpl(L"DcRemover")
{
	LOG_TRACE(L"DcRemover constructor called.", L"BasicRecon");
	AddInput(L"Input", 2, DataTypeComplexDouble | DataTypeComplexFloat);
	
	AddProperty<bool>(L"Inplace", true, L"If the processed data will be stored in place.");
	AddProperty<int>(L"CornerSize", 10, L"Size of the corners used to estimate noise level.");

	AddOutput(L"Output", 2, DataTypeComplexDouble | DataTypeComplexFloat);
}

Yap::DcRemover::DcRemover(const DcRemover& rhs)
	:ProcessorImpl(rhs)
{
	LOG_TRACE(L"DcRemover constructor called.", L"BasicRecon");
}

DcRemover::~DcRemover()
{
}

bool DcRemover::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	if (data->GetDataType() != DataTypeComplexDouble && data->GetDataType() != DataTypeComplexFloat)
		return false;

	DataHelper input_data(data);
	if (input_data.GetActualDimensionCount() != 2)
		return false;

	unsigned int width = input_data.GetWidth();
	unsigned int height = input_data.GetHeight();
	auto inplace = GetProperty<bool>(L"Inplace");
	unsigned int corner_size = GetProperty<int>(L"CornerSize");
	if (corner_size >= height / 2 || corner_size >= width / 2 || corner_size < 2)
		return false;

	if (data->GetDataType() == DataTypeComplexFloat)
	{
		if (inplace)
		{
			RemoveDC<complex<float>>(GetDataArray<complex<float>>(data),
				nullptr, width, height, inplace, corner_size);
			Feed(L"Output", data);
		}
		else
		{
			auto output_data = CreateData<complex<float>>(data);

			RemoveDC(GetDataArray<complex<float>>(data), GetDataArray<complex<float>>(output_data.get()),
				width, height, inplace, corner_size);
			Feed(L"Output", output_data.get());
		}

	}
	else // DataTypeComplexDouble
	{
		if (inplace)
		{
			RemoveDC<complex<double>>(GetDataArray<complex<double>>(data),
				nullptr, width, height, inplace, corner_size);
			Feed(L"Output", data);
		}
		else
		{
			auto output_data = CreateData<complex<double>>(data);

			RemoveDC(GetDataArray<complex<double>>(data), GetDataArray<complex<double>>(output_data.get()),
				width, height, inplace, corner_size);
			Feed(L"Output", output_data.get());
		}
	}

	return true;
}
