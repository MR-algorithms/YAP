#include "stdafx.h"
#include "Fft1D.h"
#include <string>

#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")

using namespace std;
CFft1D::CFft1D()
{
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

	CData raw_data(data);
	if (raw_data.GetDataType() != DataTypeComplexDouble)
		return false;
	if (raw_data.GetDimensionCount() != 1)
		return false;

	Fft1D(data);

	return true;
}

wchar_t * CFft1D::GetId()
{
	return L"Fft1D";
}


std::vector<std::complex<double>> CFft1D::Transform(const std::complex<double>* input)
{
	vector<complex<double>> output;
	output.resize(_width);

	fftw_execute_dft(_fft_plan, (fftw_complex*)input, (fftw_complex*)output.data());

	return output;
}

void CFft1D::FFTShift(std::vector<std::complex<double>>& data)
{
	unsigned int width = static_cast<unsigned int> (data.size());
	bool is_odd = (width % 2 == 0 ? false : true);
	if (is_odd)
	{
		SwapBlock(data.data(), data.data() + (width-1) / 2 + 1, (width-1) / 2);
	}
	else
	{
		SwapBlock(data.data(), data.data() + width / 2, width / 2);
	}
}

void CFft1D::SwapBlock(std::complex<double>* block1, std::complex<double>* block2, unsigned int width)
{
	std::vector<std::complex<double>> swap_buffer;
	swap_buffer.resize(width);

	auto cursor1 = block1;
	auto cursor2 = block2;

	memcpy(swap_buffer.data(), cursor1, width * sizeof(std::complex<double>));
	memcpy(cursor1, cursor2, width * sizeof(std::complex<double>));
	memcpy(cursor2, swap_buffer.data(), width * sizeof(std::complex<double>));

}

bool CFft1D::Fft1D(IData* data)
{
	CData raw_data(data);
	CFft1D Fft1;
	unsigned int _width = raw_data.GetWidth();
	vector<complex<double>> result(_width);	
	auto rawdata = reinterpret_cast<complex<double>* > (raw_data.GetData());
	_fft_plan = fftw_plan_dft_1d(_width, (fftw_complex*)rawdata, 
		(fftw_complex*)result.data(), FFTW_BACKWARD, FFTW_ESTIMATE);

	result = Fft1.Transform(rawdata);

	for (auto data : result)
	{
		data /= sqrt(result.size());
	}

	Fft1.FFTShift(result);
	memcpy(rawdata, result.data(), result.size() * sizeof(std::complex<double>));

	return true;
}
