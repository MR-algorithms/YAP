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

	Fft1D(data);

	return true;
}

wchar_t * CFft1D::GetId()
{
	return L"Fft1D";
}


std::vector<std::complex<double>> CFft1D::Transform(const std::vector<std::complex<double>>& input)
{
	vector<complex<double>> output;
	output.resize(input.size());

	fftw_execute_dft(_fft_plan, (fftw_complex*)input.data(), (fftw_complex*)output.data());

	return output;
}

void CFft1D::FFTShift(std::vector<std::complex<double>>& data)
{
	unsigned int width = data.size();
	bool is_odd = (width % 2 == 0 ? false : true);
	std::vector<std::complex<double>> temp(width);

	TODO(Use only half buffer size.);
	if (is_odd)
	{
		copy(data.begin(), data.begin() + width / 2, temp.begin() + width / 2 + 1);
		copy(data.begin() + width / 2, data.end(), temp.begin());
	}
	else
	{
		copy(data.begin(), data.begin() + width / 2, temp.begin() + width / 2);
		copy(data.begin() + width / 2, data.end(), temp.begin());
	}
	copy(temp.begin(), temp.end(), data.begin());
}

bool CFft1D::Fft1D(IData* data)
{
	CData raw_data(data);
	if (raw_data.GetDataType() != DataTypeComplexDouble)
		return false;
	if (raw_data.GetDimensionCount() != 1)
		return false;

	CFft1D Fft1;
	vector<complex<double>> recon_data(raw_data.GetWidth());
	vector<complex<double>> result(raw_data.GetWidth());

	unsigned int width = raw_data.GetWidth();
	auto rawdata = reinterpret_cast<complex<double>* > (raw_data.GetData());
	_fft_plan = fftw_plan_dft_1d(width, (fftw_complex*)rawdata, 
		(fftw_complex*)result.data(), FFTW_BACKWARD, FFTW_ESTIMATE);

	BUG(Change the parameter type of Transform());
	//result = Fft1.Transform(rawdata);

	for (auto data : result)
	{
		data /= result.size();
	}

	Fft1.FFTShift(result);

	copy(result.begin(), result.end(), rawdata);

	return true;
}
