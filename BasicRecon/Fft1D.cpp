#include "stdafx.h"
#include "Fft1D.h"
#include <string>

#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")

using namespace std;
using namespace Yap;

CFft1D::CFft1D() :
	_plan_data_size(0),
	_plan_inverse(false),
	_plan_in_place(false)
{
	AddProperty(PropertyBool, L"Inverse", L"");
	AddProperty(PropertyBool, L"InPlace", L"");

	SetBoolProperty(L"Inverse", false);
	SetBoolProperty(L"InPlace", true);

	AddInputPort(L"Input", 1, DataTypeComplexDouble);
	AddOutputPort(L"Output", 1, DataTypeComplexDouble);
}

CFft1D::~CFft1D()
{
}

bool CFft1D::Init()
{
	return true;
}

bool CFft1D::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	CDataHelper raw_data(data);
	if (raw_data.GetDataType() != DataTypeComplexDouble)
		return false;
	if (raw_data.GetDimensionCount() != 1)
		return false;

	if (GetBoolProperty(L"InPlace"))
	{
		Fft1D(reinterpret_cast<complex<double>*>(raw_data.GetData()),
			reinterpret_cast<complex<double>*>(raw_data.GetData()),
			raw_data.GetWidth(), GetBoolProperty(L"Inverse"));
		Feed(L"Output", data);
	}
	else
	{
		auto * output_data = new Yap::CDoubleData(data->GetDimension());
		Fft1D(reinterpret_cast<complex<double>*>(raw_data.GetData()),
			reinterpret_cast<complex<double>*>(output_data->GetData()),
			raw_data.GetWidth(), GetBoolProperty(L"Inverse"));
		Feed(L"Output", output_data);
	}

	return true;
}

wchar_t * CFft1D::GetId()
{
	return L"Fft1D";
}

void CFft1D::FFTShift(std::complex<double>* data, size_t size)
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

void CFft1D::SwapBlock(std::complex<double>* block1, 
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

void CFft1D::Plan(size_t size, bool inverse, bool in_place)
{
	vector<fftw_complex> data(size);
	if (in_place)
	{
		_fft_plan = fftw_plan_dft_1d(_plan_data_size, (fftw_complex*)data.data(),
			(fftw_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}
	else
	{
		vector<fftw_complex> result(size);
		_fft_plan = fftw_plan_dft_1d(_plan_data_size, (fftw_complex*)data.data(),
			(fftw_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}

	_plan_data_size = static_cast<unsigned int> (size);
	_plan_inverse = inverse;
	_plan_in_place = in_place;
}

bool CFft1D::Fft1D(std::complex<double> * data, 
	std::complex<double> * result_data,
	size_t size, 
	bool inverse)
{
	bool in_place = (data == result_data);

	if (size != _plan_data_size || inverse != _plan_inverse || in_place != _plan_in_place)
	{
		Plan(size, inverse, in_place);
	}

	fftw_execute_dft(_fft_plan, (fftw_complex*)data, (fftw_complex*)result_data);

	for (auto data = result_data; data < result_data + size; ++data)
	{
		*data /= sqrt(size);
	}

	FFTShift(result_data, size);

	return true;
}
