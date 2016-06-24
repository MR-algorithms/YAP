#include "stdafx.h"
#include "Fft2D.h"
#include <string>
#include "..\Interface\SmartPtr.h"
#include "DataHelper.h"
#include "..\Interface\ReconData.h"

using namespace std;
using namespace Yap;

CFft2D::CFft2D():
	CProcessorImp(L"Fft2D"),
	_plan_data_width(0),
	_plan_data_height(0),
	_plan_inverse(false),
	_plan_in_place(false)
{
	AddProperty(PropertyBool, L"Inverse", L"");
	AddProperty(PropertyBool, L"InPlace", L"");

	SetBoolProperty(L"Inverse", false);
	SetBoolProperty(L"InPlace", true);

	AddInputPort(L"Input", 2, DataTypeComplexDouble);
	AddOutputPort(L"Output", 2, DataTypeComplexDouble);
}


CFft2D::~CFft2D()
{
}

bool CFft2D::Init()
{
	return true;
}

bool CFft2D::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	CDataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble)
		return false;
	if (input_data.GetDimensionCount() != 2)
		return false;

	auto width = input_data.GetWidth();
	auto height = input_data.GetHeight();

	if (GetBoolProperty(L"InPlace"))
	{

		Fft2D(reinterpret_cast<complex<double>*>(input_data.GetData()),
			reinterpret_cast<complex<double>*>(input_data.GetData()),
			width, height, GetBoolProperty(L"Inverse"));
		Feed(L"Output", data);
	}
	else
	{

		Yap::CDimensions dims;
		dims(DimensionReadout, 0, width)
			(DimensionPhaseEncoding, 0, height);
		auto output = CSmartPtr<CComplexDoubleData>(new CComplexDoubleData(&dims));
		Fft2D(reinterpret_cast<complex<double>*>(input_data.GetData()),
			reinterpret_cast<complex<double>*>(output->GetData()),
			width, height, GetBoolProperty(L"Inverse"));
		Feed(L"Output", output.get());
	}
	return true;
}

void CFft2D::FftShift(std::complex<double>* data, size_t  width, size_t height)
{
	SwapBlock(data, data + height / 2 * width + width / 2, width / 2, height / 2, width);
	SwapBlock(data + width / 2, data + height / 2 * width, width / 2, height / 2, width);
}

void CFft2D::SwapBlock(std::complex<double>* block1, std::complex<double>* block2, size_t width, size_t height, size_t line_stride)
{
	std::vector<std::complex<double>> swap_buffer;
	swap_buffer.resize(width);

	auto cursor1 = block1;
	auto cursor2 = block2;
	for (unsigned int row = 0; row < height; ++row)
	{
		memcpy(swap_buffer.data(), cursor1, width * sizeof(std::complex<double>));
		memcpy(cursor1, cursor2, width * sizeof(std::complex<double>));
		memcpy(cursor2, swap_buffer.data(), width * sizeof(std::complex<double>));

		cursor1 += line_stride;
		cursor2 += line_stride;
	}
}

bool CFft2D::Fft2D(std::complex<double> * data, std::complex<double> * result_data, size_t width, size_t height, bool inverse)
{
	
	bool in_place = (data == result_data);
	if (width != _plan_data_width || height != _plan_data_height || inverse != _plan_inverse || in_place != _plan_in_place)
	{
		Plan(width, height, inverse, in_place);
	}
	fftw_execute_dft(_fft_plan, (fftw_complex*)data, (fftw_complex*)result_data);

	for (auto data = result_data; data < result_data + width * height; ++data)
	{
		*data /=  sqrt(width * height);
	}

	FftShift(result_data, width, height);

	return true;
}

void CFft2D::Plan(size_t width, size_t height, bool inverse, bool in_place)
{
	vector<fftw_complex> data(width * height);
	if (in_place)
	{
		_fft_plan = fftw_plan_dft_2d(_plan_data_width, _plan_data_height, (fftw_complex*)data.data(),
			(fftw_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}
	else
	{
		vector<fftw_complex> result(width * height);
		_fft_plan = fftw_plan_dft_2d(_plan_data_width, _plan_data_height,  (fftw_complex*)data.data(),
			(fftw_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}

	_plan_data_width = static_cast<unsigned int> (width);
	_plan_data_height = static_cast<unsigned int> (height);
	_plan_inverse = inverse;
	_plan_in_place = in_place;
}
