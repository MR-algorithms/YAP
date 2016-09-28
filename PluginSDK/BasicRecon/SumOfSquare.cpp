#include "stdafx.h"
#include "SumOfSquare.h"
#include "Interface/Client/DataHelper.h"


using namespace std;
using namespace Yap;


SumOfSquare::SumOfSquare():
	ProcessorImpl(L"SumOfSquare")
{
}


SumOfSquare::~SumOfSquare()
{
}

bool Yap::SumOfSquare::OnInit()
{
	AddInput(L"Input", 3, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeFloat);

	return true;
}

IProcessor * Yap::SumOfSquare::Clone()
{
	return new (nothrow) SumOfSquare(*this);
}

bool Yap::SumOfSquare::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	if (data->GetDataType() != DataTypeComplexDouble && data->GetDataType() != DataTypeComplexFloat)
		return false;

	DataHelper input_data(data);
	auto width = input_data.GetWidth();
	auto height = input_data.GetHeight();
	auto coil_count = input_data.GetCoilCount();

	auto recon_data = SumofSquare(GetDataArray<complex<float>>(data), width, height, coil_count);

	float * output_data = nullptr;
	try
	{
		output_data = new float[width * height];
	}
	catch (const std::exception&)
	{
		return false;
	}

	memcpy(output_data, recon_data.data(), width * height * sizeof(float));
	Dimensions dimensions;
	dimensions(DimensionReadout, 0U, width)
		(DimensionPhaseEncoding, 0U, height);
	auto outdata = YapShared(new FloatData(output_data, dimensions, nullptr, true));
	Feed(L"Output", outdata.get());

	return true;
}

std::vector<float> Yap::SumOfSquare::SumofSquare(std::complex<float>* input_data, unsigned int width, unsigned int height, unsigned int coil_count)
{
	std::vector<float> recon_data(width * height);
	for (unsigned int i = 0; i < width * height; ++i)
	{
		for (unsigned int c = 0; c < coil_count; ++c)
		{
			*(recon_data.data() + i) += abs(*(input_data + c * width * height + i)) * abs(*(input_data + c * width * height + i));
		}
		*(recon_data.data() + i) = sqrt(*(recon_data.data() + i));
	}
	return recon_data;
}
