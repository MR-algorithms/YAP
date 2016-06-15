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

	Fft1D(data, 2);

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

	fftw_execute_dft(_p_ifft, (fftw_complex*)input.data(), (fftw_complex*)output.data());

	return output;
}

void CFft1D::FFTShift(std::vector<std::complex<double>>& data)
{
	unsigned int width = data.size();
	bool is_odd = (width % 2 == 0 ? false : true);
	std::vector<std::complex<double>> temp;
	temp.resize(width);

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

bool CFft1D::Fft1D(IData* data, unsigned int height_index)
{
	CData raw_data(data);
	if (raw_data.GetDataType() != DataTypeComplexDouble)
		return false;
	if (raw_data.GetDimensionCount() != 1)
		return false;

	CFft1D Fft1;
	vector<complex<double>> recon_data;
	recon_data.resize(raw_data.GetWidth());
	vector<complex<double>> result;
	result.resize(raw_data.GetWidth());
	unsigned int width = raw_data.GetWidth();
	auto rawdata = reinterpret_cast<vector<complex<double>>* > (raw_data.GetData());
	fftw_plan _p_ifft = fftw_plan_dft_1d(width, (fftw_complex*)rawdata, (fftw_complex*)result.data(), FFTW_BACKWARD, FFTW_ESTIMATE);
	result = Fft1.Transform(*rawdata);
	std::complex<double> scale(result.size(), 0);
	for (auto iter = result.begin(); iter != result.end(); ++iter)
	{
		*iter = *iter / scale;
	}
	Fft1.FFTShift(result);
	unsigned int start_index =  height_index * width;
	copy(result.begin(), result.end(), recon_data.begin() + start_index);

	return true;
}
