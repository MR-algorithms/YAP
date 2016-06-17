#include "stdafx.h"
#include "Fft2D.h"
#include <string>


using namespace std;

CFft2D::CFft2D()
{
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
	CData raw_data(data);
	if (raw_data.GetDataType() != DataTypeComplexDouble)
		return false;
	if (raw_data.GetDimensionCount() != 2)
		return false;

 	Fft2D(data);
	return true;
}

wchar_t * CFft2D::GetId()
{
	return L"Fft2D";
}

void CFft2D::SetFFTPlan(const fftw_plan & plan)
{
	_fft_plan = plan;
}

std::vector<std::complex<double>> CFft2D::Transform(std::complex<double>* input)
{
	vector<complex<double>> output;
	output.resize(_width, _height);

	fftw_execute_dft(_fft_plan, (fftw_complex*)input, (fftw_complex*)output.data());

	return output;
}

void CFft2D::FftShift(std::vector<std::complex<double>>& data, unsigned int width, unsigned int height)
{
	SwapBlock(data.data(), data.data() + height / 2 * width + width / 2, width / 2, height / 2, width);
	SwapBlock(data.data() + width / 2, data.data() + height / 2 * width, width / 2, height / 2, width);
}

void CFft2D::SwapBlock(std::complex<double>* block1, std::complex<double>* block2, unsigned int width, unsigned int height, unsigned int line_stride)
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

bool CFft2D::Fft2D(IData * data)
{
	CData raw_data(data);
	CFft2D Fft2;
	unsigned int _width = raw_data.GetWidth();
	unsigned int _height = raw_data.GetHeight();

	vector<complex<double>> result(_width, _height);

	auto rawdata = reinterpret_cast<complex<double>*> (raw_data.GetData());
	_fft_plan = fftw_plan_dft_2d(_height, _width, (fftw_complex*)rawdata, (fftw_complex*)rawdata, FFTW_BACKWARD, FFTW_ESTIMATE);
	Fft2.SetFFTPlan(_fft_plan);
	result = Fft2.Transform(rawdata);

	for (auto data : result)
	{
		data /=  sqrt(result.size());
	}

	Fft2.FftShift(result, _width, _height);
	memcpy(rawdata, result.data(), result.size() * sizeof(std::complex<double>));

	return true;
}
