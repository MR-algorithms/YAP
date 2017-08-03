#include "Fft2D.h"

#include "Client/DataHelper.h"
#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"

#include <string>

using namespace std;
using namespace Yap;

Fft2D::Fft2D():
	ProcessorImpl(L"Fft2D"),
	_plan_data_width(0),
	_plan_data_height(0),
	_plan_inverse(false),
	_plan_in_place(false),
	_fft_plan(nullptr)
{
	LOG_TRACE(L"Fft2D constructor called.", L"BasicRecon");
	AddProperty<bool>( L"Inverse", false, L"The direction of FFT2D.");
	AddProperty<bool>( L"InPlace", true, L"The position of FFT2D.");

	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);
}


Fft2D::Fft2D(const Fft2D& rhs)
	:ProcessorImpl(rhs),
	_plan_data_width(rhs._plan_data_width),
	_plan_data_height(rhs._plan_data_height),
	_plan_inverse(rhs._plan_inverse),
	_plan_in_place(rhs._plan_in_place),
	_fft_plan(rhs._fft_plan)
{
	LOG_TRACE(L"Fft2D constructor called.", L"BasicRecon");
}


Fft2D::~Fft2D()
{
	LOG_TRACE(L"Fft2D destructor called.", L"BasicRecon");
}

bool Fft2D::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	DataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexFloat )
		return false;

	if (input_data.GetActualDimensionCount() != 2)
		return false;

	auto width = input_data.GetWidth();
	auto height = input_data.GetHeight();

	auto data_array = GetDataArray<complex<float>>(data);

	if (GetProperty<bool>(L"InPlace"))
	{
		Fft(data_array, data_array, width, height, GetProperty<bool>(L"Inverse"));
		Feed(L"Output", data);
	}
	else
	{
		auto output = CreateData<complex<double>>(data);

		Fft(data_array, GetDataArray<complex<float>>(output.get()),
			width, height, GetProperty<bool>(L"Inverse"));
		Feed(L"Output", output.get());
	}
	return true;
}

void Fft2D::FftShift(std::complex<float>* data, size_t  width, size_t height)
{
	SwapBlock(data, data + height / 2 * width + width / 2, width / 2, height / 2, width);
	SwapBlock(data + width / 2, data + height / 2 * width, width / 2, height / 2, width);
}

void Fft2D::SwapBlock(std::complex<float>* block1, std::complex<float>* block2, size_t width, size_t height, size_t line_stride)
{
	std::vector<std::complex<float>> swap_buffer;
	swap_buffer.resize(width);

	auto cursor1 = block1;
	auto cursor2 = block2;
	for (unsigned int row = 0; row < height; ++row)
	{
		memcpy(swap_buffer.data(), cursor1, width * sizeof(std::complex<float>));
		memcpy(cursor1, cursor2, width * sizeof(std::complex<float>));
		memcpy(cursor2, swap_buffer.data(), width * sizeof(std::complex<float>));

		cursor1 += line_stride;
		cursor2 += line_stride;
	}
}

bool Fft2D::Fft(std::complex<float> * data, std::complex<float> * result_data, size_t width, size_t height, bool inverse)
{
	
	bool in_place = (data == result_data);
	if (width != _plan_data_width || height != _plan_data_height || inverse != _plan_inverse || in_place != _plan_in_place)
	{
		Plan(width, height, inverse, in_place);
	}
	fftwf_execute_dft(_fft_plan, (fftwf_complex*)data, (fftwf_complex*)result_data);

	for (auto data = result_data; data < result_data + width * height; ++data)
	{
		*data /=  float(sqrt(width * height));
	}

	FftShift(result_data, width, height);

	return true;
}

void Fft2D::Plan(size_t width, size_t height, bool inverse, bool in_place)
{
	vector<fftwf_complex> data(width * height);

	if (in_place)
	{
		_fft_plan = fftwf_plan_dft_2d(int(width), int(height), (fftwf_complex*)data.data(),
			(fftwf_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}
	else
	{
		vector<fftwf_complex> result(width * height);
		_fft_plan = fftwf_plan_dft_2d(int(width), int(height),  (fftwf_complex*)data.data(),
			(fftwf_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}

	_plan_data_width = static_cast<unsigned int> (width);
	_plan_data_height = static_cast<unsigned int> (height);
	_plan_inverse = inverse;
	_plan_in_place = in_place;
}

