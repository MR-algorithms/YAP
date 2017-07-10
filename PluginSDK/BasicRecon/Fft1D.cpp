#include "Fft1D.h"

#include "Client/DataHelper.h"
#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"

#include <string.h>

#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")

using namespace std;
using namespace Yap;

Fft1D::Fft1D() : 
	ProcessorImpl(L"Fft1D"),
	_plan_data_size(0),
	_plan_inverse(false),
	_plan_in_place(false)
{
	LOG_TRACE(L"Fft1D constructor called.", L"BasicRecon");
	AddProperty<bool>(L"Inverse", false, L"The direction of FFT1D.");
	AddProperty<bool>(L"InPlace", true, L"The position of FFT1D.");

	AddInput(L"Input", 1, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", 1, DataTypeComplexDouble | DataTypeComplexFloat);
}


Fft1D::Fft1D(const Fft1D& rhs)
	:ProcessorImpl(rhs),
	_plan_data_size(rhs._plan_data_size),
	_plan_inverse(rhs._plan_inverse),
	_plan_in_place(rhs._plan_in_place)
{
	LOG_TRACE(L"Fft1D constructor called.", L"BasicRecon");
}

Fft1D::~Fft1D()
{
	LOG_TRACE(L"Fft1D destructor called.", L"BasicRecon");
}

template<typename T> bool Fft1D::DoFft(IData * data, size_t size)
{
	auto data_array = GetDataArray<complex<T>>(data);
	if (GetProperty<bool>(L"InPlace"))
	{
		Fft<T>(data_array, data_array, size, GetProperty<bool>(L"Inverse"));
		return Feed(L"Output", data);
	}
	else
	{
		Yap::Dimensions dims;
		dims(DimensionReadout, 0, size);
		auto output = CreateData<complex<T>>(&dims);
		Fft<T>(data_array, GetDataArray<complex<T>>(output.get()), size, GetProperty<bool>(L"Inverse"));
		return Feed(L"Output", output.get());
	}
}

bool Fft1D::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	DataHelper input_data(data);
	if (input_data.GetActualDimensionCount() != 1)
		return false;

	if ((input_data.GetDataType() & DataTypeComplexDouble) != 0)
	{
		return DoFft<double>(data, input_data.GetWidth());
	}
	else if ((input_data.GetDataType() & DataTypeComplexFloat) != 0)
	{
		return DoFft<float>(data, input_data.GetWidth());
	}
	else
	{
		return false;
	}
}

template <typename T>
void Fft1D::FFTShift(std::complex<T>* data, size_t size)
{
	bool is_odd = ((size % 2) != 0);
	if (is_odd)
	{
		SwapBlock(data, data + (size-1) / 2 + 1, (size-1) / 2);
	}
	else
	{
		SwapBlock(data, data + size / 2, size / 2);
	}
}

template <typename T>
void Fft1D::SwapBlock(std::complex<T>* block1, 
	std::complex<T>* block2, 
	size_t width)
{
	std::vector<std::complex<T>> swap_buffer;
	swap_buffer.resize(width);

	auto cursor1 = block1;
	auto cursor2 = block2;

	memcpy(swap_buffer.data(), cursor1, width * sizeof(std::complex<T>));
	memcpy(cursor1, cursor2, width * sizeof(std::complex<T>));
	memcpy(cursor2, swap_buffer.data(), width * sizeof(std::complex<T>));
}

template <>
void Fft1D::Plan<double>(size_t size, bool inverse, bool in_place)
{
	vector<fftw_complex> data(size);
	if (in_place)
	{
		_fft_plan_double = fftw_plan_dft_1d(int(size), (fftw_complex*)data.data(),
			(fftw_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}
	else
	{
		vector<fftw_complex> result(size);
		_fft_plan_double = fftw_plan_dft_1d(int(size), (fftw_complex*)data.data(),
			(fftw_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}

	_plan_data_size = static_cast<unsigned int> (size);
	_plan_inverse = inverse;
	_plan_in_place = in_place;
}

template <>
void Fft1D::Plan<float>(size_t size, bool inverse, bool in_place)
{
	vector<fftwf_complex> data(size);
	if (in_place)
	{
		_fft_plan_float = fftwf_plan_dft_1d(int(size), (fftwf_complex*)data.data(),
			(fftwf_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}
	else
	{
		vector<fftwf_complex> result(size);
		_fft_plan_float = fftwf_plan_dft_1d(int(size), (fftwf_complex*)data.data(),
			(fftwf_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}

	_plan_data_size = static_cast<unsigned int> (size);
	_plan_inverse = inverse;
	_plan_in_place = in_place;
}

template<>
bool Fft1D::Fft(std::complex<double> * data, 
				std::complex<double> * result_data,
				size_t size,
				bool inverse)
{
	bool in_place = (data == result_data);

	if (size != _plan_data_size || inverse != _plan_inverse || in_place != _plan_in_place)
	{
		Plan<double>(size, inverse, in_place);
	}

	fftw_execute_dft(_fft_plan_double, (fftw_complex*)data, (fftw_complex*)result_data);

	for (auto data = result_data; data < result_data + size; ++data)
	{
		*data /= sqrt(size);
	}

	FFTShift(result_data, size);

	return true;
}

template<>
bool Fft1D::Fft(std::complex<float> * data,
	std::complex<float> * result_data,
	size_t size,
	bool inverse)
{
	bool in_place = (data == result_data);

	if (size != _plan_data_size || inverse != _plan_inverse || in_place != _plan_in_place)
	{
		Plan<float>(size, inverse, in_place);
	}

	fftwf_execute_dft(_fft_plan_float, (fftwf_complex*)data, (fftwf_complex*)result_data);

	for (auto data = result_data; data < result_data + size; ++data)
	{
		*data /= static_cast<float>(sqrt(size));
	}

	FFTShift(result_data, size);

	return true;
}

