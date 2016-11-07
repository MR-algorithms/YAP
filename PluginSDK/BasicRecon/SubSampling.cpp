#include "stdafx.h"
#include "SubSampling.h"
#include "Interface/Client/DataHelper.h"

using namespace std;
using namespace Yap;

template <typename T>
void GetSubSampledData(T * input_data, T * mask, T * output_data, unsigned int width, unsigned int height)
{
	for (unsigned int i = 0; i < width * height; ++i)
	{
		*(output_data + i) = *(input_data + i) * *(mask + i);
	}
}

SubSampling::SubSampling():
	ProcessorImpl(L"SubSampling")
{
}

Yap::SubSampling::SubSampling(const SubSampling & rhs)
	: ProcessorImpl(rhs)
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddInput(L"Mask", 2, DataTypeChar);
	AddOutput(L"Output", 2, DataTypeComplexDouble | DataTypeComplexFloat);
}

SubSampling::~SubSampling()
{
}


IProcessor * Yap::SubSampling::Clone()
{
	return new (nothrow) SubSampling(*this);
}

bool Yap::SubSampling::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) == L"Mask")
	{
		_mask = YapShared(data);
	}
	else if (wstring(port) == L"Input")
	{
		if (!_mask)
			return false;
		DataHelper input_data(data);
		DataHelper mask(_mask.get());

		auto width = input_data.GetWidth();
		auto height = input_data.GetHeight();

		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, width)
			(DimensionPhaseEncoding, 0U, height);

		auto outdata = YapShared(new ComplexFloatData(&dimensions));

		GetSubSampledData(GetDataArray<complex<float>>(data), GetDataArray<complex<float>>(_mask.get()),
			GetDataArray<complex<float>>(outdata.get()), width, height);

		Feed(L"Output", outdata.get());
	}

	return true;
}
