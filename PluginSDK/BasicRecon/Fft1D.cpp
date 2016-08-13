#include "Fft1D.h"

#include "Interface/SmartPtr.h"
#include "Yap/DataHelper.h"
#include "Interface/DataImp.h"

#include <string.h>

#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")

using namespace std;
using namespace Yap;

CFft1D::CFft1D() : 
	CProcessorImp(L"Fft1D"),
	_plan_data_size(0),
	_plan_inverse(false),
	_plan_in_place(false)
{
	AddProperty(PropertyBool, L"Inverse", L"The direction of FFT1D."); 
	AddProperty(PropertyBool, L"InPlace", L"The position of FFT1D.");

	SetBool(L"Inverse", false);
	SetBool(L"InPlace", true);

	AddInputPort(L"Input", 1, DataTypeComplexDouble);
	AddOutputPort(L"Output", 1, DataTypeComplexDouble);
}

CFft1D::~CFft1D()
{
}

bool CFft1D::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	CDataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble)
		return false;

	if (input_data.GetActualDimensionCount() != 1)
		return false;

	auto size = input_data.GetWidth();

	if (GetBool(L"InPlace"))
	{
		Fft1D(reinterpret_cast<complex<double>*>(input_data.GetData()),
			reinterpret_cast<complex<double>*>(input_data.GetData()),
			size, GetBool(L"Inverse"));
		Feed(L"Output", data);
	}
	else
	{
		Yap::CDimensionsImp dims;
		dims(DimensionReadout, 0, size);
		auto output = CSmartPtr<CComplexDoubleData>(new CComplexDoubleData(&dims));
		Fft1D(reinterpret_cast<complex<double>*>(input_data.GetData()),
			reinterpret_cast<complex<double>*>(output->GetData()),
			size, GetBool(L"Inverse"));
		Feed(L"Output", output.get());
	}

	return true;
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

Yap::IProcessor * Yap::CFft1D::Clone()
{
	try
	{
		return new CFft1D;
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}
