#include "stdafx.h"
#include "SubSampling.h"
#include "Client/DataHelper.h"

using namespace std;
using namespace Yap;


SubSampling::SubSampling():
	ProcessorImpl(L"SubSampling")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddInput(L"Mask", 2, DataTypeFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
}


SubSampling::~SubSampling()
{
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
			(DimensionPhaseEncoding, 0U, height)
			(DimensionSlice, 0U, 1)
			(Dimension4, 0U, 1)
			(DimensionChannel, 0U, 1);

		auto outdata = YapShared(new ComplexFloatData(&dimensions));

		GetSubSampledData(GetDataArray<complex<float>>(data), GetDataArray<float>(_mask.get()),
			GetDataArray<complex<float>>(outdata.get()), width, height);

		Feed(L"Output", outdata.get());
	}

	return true;
}

void Yap::SubSampling::GetSubSampledData(std::complex<float> * input_data, float * mask, std::complex<float> * output_data, 
	unsigned int width, unsigned int height)
{
	for (unsigned int i = 0; i < width * height; ++i)
	{
		*(output_data + i) = *(input_data + i) * *(mask + i);
	}
}
