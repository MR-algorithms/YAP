#include "stdafx.h"
#include "Fft3D.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"
#include <string>

using namespace std;
using namespace Yap;

Fft3D::Fft3D() :
	ProcessorImpl(L"Fft3D"),
	_plan_data_width(0),
	_plan_data_height(0),
	_plan_data_depth(0),
	_plan_inverse(false),
	_plan_in_place(false),
	_fft_plan(nullptr)
{
	LOG_TRACE(L"Fft3D constructor called.", L"BasicRecon");
	AddProperty<bool>(L"Inverse", false, L"The direction of FFT3D.");
	AddProperty<bool>(L"InPlace", true, L"The position of FFT3D.");

	AddInput(L"Input", 3, DataTypeComplexFloat);
	AddOutput(L"Output", 3, DataTypeComplexFloat);
}


Fft3D::Fft3D(const Fft3D& rhs)
	:ProcessorImpl(rhs),
	_plan_data_width(rhs._plan_data_width),
	_plan_data_height(rhs._plan_data_height),
	_plan_data_depth(rhs._plan_data_depth),
	_plan_inverse(rhs._plan_inverse),
	_plan_in_place(rhs._plan_in_place),
	_fft_plan(rhs._fft_plan)
{
	LOG_TRACE(L"Fft3D copy constructor called.", L"BasicRecon");
}


Fft3D::~Fft3D()
{
}

bool Fft3D::Input(const wchar_t * port, IData * data)
{
	// Do some check.
	if (data == nullptr)
	{
		LOG_ERROR(L"<Fft3D> Invalid input data!", L"BasicRecon");
		return false;
	}
	if (_wcsicmp(port, L"Input") != 0)
	{
		LOG_ERROR(L"<Fft3D> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper input_data(data);
	if (input_data.GetActualDimensionCount() > 3)
	{
		LOG_ERROR(L"<Fft3D> Error input data dimention!(3D data is available)!", L"BasicRecon");
		return false;
	}
	if (input_data.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<Fft3D> Error input data type!(DataTypeComplexFloat is available)!", L"BasicRecon");
		return false;
	}

	auto width = input_data.GetWidth();
	auto height = input_data.GetHeight();
	auto depth = input_data.GetSliceCount();

	auto data_array = GetDataArray<complex<float>>(data);

	if (GetProperty<bool>(L"InPlace"))
	{
		Fft(data_array, data_array, width, height, depth, GetProperty<bool>(L"Inverse"));
		return Feed(L"Output", data);
	}
	else
	{
		auto output = CreateData<complex<double>>(data);

		Fft(data_array, GetDataArray<complex<float>>(output.get()),
			width, height, depth, GetProperty<bool>(L"Inverse"));
		return Feed(L"Output", output.get());
	}
}

bool Fft3D::Fft(std::complex<float> * data, std::complex<float> * result_data,
	size_t width, size_t height, size_t depth, bool inverse)
{

	bool in_place = (data == result_data);
	if (width != _plan_data_width || 
		height != _plan_data_height || 
		depth != _plan_data_depth || 
		inverse != _plan_inverse || 
		in_place != _plan_in_place)
	{
		Plan(width, height, depth, inverse, in_place);
	}
	fftwf_execute_dft(_fft_plan, (fftwf_complex*)data, (fftwf_complex*)result_data);

	for (auto data = result_data; data < result_data + width * height * depth; ++data)
	{
		*data /= float(sqrt(width * height * depth));
	}

	FftShift(result_data, width, height, depth);

	return true;
}

void Fft3D::Plan(size_t width, size_t height, size_t depth, bool inverse, bool in_place)
{
	vector<fftwf_complex> data(width * height * depth);

	if (in_place)
	{
		_fft_plan = fftwf_plan_dft_3d(int(depth), int(height), int(width), 
			(fftwf_complex*)data.data(),
			(fftwf_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_UNALIGNED);
	}
	else
	{
		vector<fftwf_complex> result(width * height * depth);
		_fft_plan = fftwf_plan_dft_3d(int(depth), int(height), int(width),  
			(fftwf_complex*)data.data(),
			(fftwf_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_UNALIGNED);
	}

	_plan_data_width = static_cast<unsigned int>(width);
	_plan_data_height = static_cast<unsigned int>(height);
	_plan_data_depth = static_cast<unsigned int>(depth);
	_plan_inverse = inverse;
	_plan_in_place = in_place;
}

void Fft3D::FftShift(std::complex<float>* data, size_t  width, size_t height, size_t depth)
{
	auto size = width * height;
	auto slice_data = data;
	for (unsigned int slice = 0; slice < depth; ++slice)
	{
		slice_data = data + slice * size;
		SwapBlock(slice_data, slice_data + height / 2 * width + width / 2, width / 2, height / 2, width);
		SwapBlock(slice_data + width / 2, slice_data + height / 2 * width, width / 2, height / 2, width);
	}

	int half_depth = int(depth / 2);
	vector<std::complex<float>> slice_swap_buffer;
	slice_swap_buffer.resize(size * half_depth);
	auto cursor2 = data + size * (depth - int(depth / 2));
	memcpy(slice_swap_buffer.data(), data, size * half_depth * sizeof(std::complex<float>));
	memcpy(data, data + half_depth * size, (size * (depth - half_depth)) * sizeof(std::complex<float>));
	memcpy(data + size * (depth - half_depth), slice_swap_buffer.data(), 
			size * half_depth * sizeof(std::complex<float>));
}

void Fft3D::SwapBlock(std::complex<float>* block1, std::complex<float>* block2, size_t width, size_t height, size_t line_stride)
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