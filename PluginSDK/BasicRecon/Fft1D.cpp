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
	_properties->Add(VariableBool, L"Inverse", L"The direction of FFT1D.");
	_properties->Add(VariableBool, L"InPlace", L"The position of FFT1D.");

	_properties->Set<bool>(L"Inverse", false);
	_properties->Set<bool>(L"InPlace", true);

	AddInput(L"Input", 1, DataTypeComplexDouble);
	AddOutput(L"Output", 1, DataTypeComplexDouble);
}


Fft1D::Fft1D(const Fft1D& rhs)
	:ProcessorImpl(rhs)
{
	LOG_TRACE(L"Fft1D constructor called.", L"BasicRecon");
}

Fft1D::~Fft1D()
{
	LOG_TRACE(L"Fft1D destructor called.", L"BasicRecon");
}


bool Fft1D::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	DataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble)
		return false;

	if (input_data.GetActualDimensionCount() != 1)
		return false;

	auto size = input_data.GetWidth();
	auto data_array = GetDataArray<complex<double>>(data);
	if (_properties->Get<bool>(L"InPlace"))
	{
		Fft(data_array, data_array, size, _properties->Get<bool>(L"Inverse"));
		Feed(L"Output", data);
	}
	else
	{
		Yap::Dimensions dims;
		dims(DimensionReadout, 0, size);
		auto output = YapShared(new ComplexDoubleData(&dims));
		Fft(data_array, GetDataArray<complex<double>>(output.get()), size, _properties->Get<bool>(L"Inverse"));
		Feed(L"Output", output.get());
	}

	return true;
}

void Fft1D::FFTShift(std::complex<double>* data, size_t size)
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

void Fft1D::SwapBlock(std::complex<double>* block1, 
	std::complex<double>* block2, 
	size_t width)
{
	std::vector<std::complex<double>> swap_buffer;
	swap_buffer.resize(width);

	auto cursor1 = block1;
	auto cursor2 = block2;

	memcpy(swap_buffer.data(), cursor1, width * sizeof(std::complex<double>));
	memcpy(cursor1, cursor2, width * sizeof(std::complex<double>));
	memcpy(cursor2, swap_buffer.data(), width * sizeof(std::complex<double>));

}

void Fft1D::Plan(size_t size, bool inverse, bool in_place)
{
	vector<fftwf_complex> data(size);
	if (in_place)
	{
		_fft_plan = fftwf_plan_dft_1d(int(size), (fftwf_complex*)data.data(),
			(fftwf_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}
	else
	{
		vector<fftwf_complex> result(size);
		_fft_plan = fftwf_plan_dft_1d(int(size), (fftwf_complex*)data.data(),
			(fftwf_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}

	_plan_data_size = static_cast<unsigned int> (size);
	_plan_inverse = inverse;
	_plan_in_place = in_place;
}

bool Fft1D::Fft(std::complex<double> * data, 
				std::complex<double> * result_data,
				size_t size,
				bool inverse)
{
	bool in_place = (data == result_data);

	if (size != _plan_data_size || inverse != _plan_inverse || in_place != _plan_in_place)
	{
		Plan(size, inverse, in_place);
	}

	fftwf_execute_dft(_fft_plan, (fftwf_complex*)data, (fftwf_complex*)result_data);

	for (auto data = result_data; data < result_data + size; ++data)
	{
		*data /= sqrt(size);
	}

	FFTShift(result_data, size);

	return true;
}

