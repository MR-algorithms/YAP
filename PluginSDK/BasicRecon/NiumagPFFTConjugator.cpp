#include "stdafx.h"
#include "NiumagPFFTConjugator.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"

#include "fftw3.h"

using namespace Yap;
using namespace std; 

template <typename T>
bool Conjugate(T* input_data,
			   T* output_data,
			   int data_size,
			   int points_count,
			   int first_point_index)
{
	assert(input_data != nullptr && output_data != nullptr && 
		data_size > 0 && points_count > 0 && first_point_index >= 0);
	assert(first_point_index + points_count <= data_size);

	for (int i = 0; i < data_size; ++i)
	{
		if (i >= first_point_index && i < first_point_index + points_count)
		{
			*(output_data + i) = conj(*(input_data + (data_size - 1 - i)));
		}
		else
		{
			*(output_data + i) = *(input_data + i);
		}
	}

	return true;
}

void SwapBlock(std::complex<float>* block1, std::complex<float>* block2, size_t width, size_t height, size_t line_stride)
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

void FftShift(std::complex<float>* data, size_t  width, size_t height)
{
	SwapBlock(data, data + height / 2 * width + width / 2, width / 2, height / 2, width);
	SwapBlock(data + width / 2, data + height / 2 * width, width / 2, height / 2, width);
}

Yap::NiumagPFFTConjugator::NiumagPFFTConjugator():
	ProcessorImpl(L"NiumagPFFTConjugator")
{
	LOG_TRACE(L"NiumagPFFTConjugator constructor called.", L"BasicRecon");
	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);

	AddProperty<int>(L"FirstPointIndex", 0, L"The index of the first point you want to get.");
	AddProperty<int>(L"PointsCount", 0, L"The count of points you want to get.");
}

Yap::NiumagPFFTConjugator::NiumagPFFTConjugator(const NiumagPFFTConjugator& rhs):
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"NiumagPFFTConjugator constructor called.", L"BasicRecon");
}

Yap::NiumagPFFTConjugator::~NiumagPFFTConjugator()
{
	LOG_TRACE(L"NiumagPFFTConjugator destructor called.", L"BasicRecon");
}

bool Yap::NiumagPFFTConjugator::Input(const wchar_t * name, IData * data)
{
	if (wstring(name) != L"Input")
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper input(data);
	if (input.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Error input data type! (DataTypeComplexFloat is available)", L"BasicRecon");
		return false;
	}

	if (input.GetActualDimensionCount() != 2)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Error input data dimention! (2D data is available)", L"BasicRecon");
		return false;
	}

	auto points_count = GetProperty<int>(L"PointsCount");
	if (points_count < 0)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> PointsCount cannot less than 0!", L"BasicRecon");
		return false;
	}

	auto first_point_index = GetProperty<int>(L"FirstPointIndex");
	if (first_point_index < 0)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> FirstPointIndex cannot less than 0!", L"BasicRecon");
		return false;
	}

	auto data_size = input.GetWidth() * input.GetHeight();
	if (points_count == 0)
	{
		points_count = data_size;
		first_point_index = 0;
	}
	if (first_point_index + points_count > (int)data_size)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Improper properties, the sum of PointsCount and FirstPointIndex should be less than (or equal) data size!", L"BasicRecon");
		return false;
	}

	auto input_data = GetDataArray<complex<float>>(data);
	if (input_data == nullptr)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Input data is nullptr!", L"BasicRecon");
		return false;
	}

	auto output = CreateData<complex<float>>(data);
	auto output_data = GetDataArray<complex<float>>(output.get());
	if (output_data == nullptr)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Create output data failed!", L"BasicRecon");
		return false;
	}

	// FFT2D
	auto forward_plan = fftwf_plan_dft_2d(int(input.GetHeight()), int(input.GetWidth()),
										(fftwf_complex*)input_data,
										(fftwf_complex*)output_data,
										FFTW_BACKWARD, FFTW_ESTIMATE);
	fftwf_execute(forward_plan);
	for (auto iter = output_data; iter < output_data + data_size; ++iter)
	{
		(*iter) /= float(sqrt(data_size));
	}

	FftShift(output_data, input.GetWidth(), input.GetHeight());

	// conjugate
	auto temp = output;
	if (!Conjugate(GetDataArray<complex<float>>(temp.get()), output_data, data_size, points_count, first_point_index))
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Failed when conjugate data!", L"BasicRecon");
		return false;
	}

	// inverse FFT2D
	auto backward_plan = fftwf_plan_dft_2d(int(input.GetHeight()), int(input.GetWidth()),
										(fftwf_complex*)output_data,
										(fftwf_complex*)output_data,
										FFTW_FORWARD, FFTW_ESTIMATE);
	fftwf_execute(backward_plan);
	for (auto iter = output_data; iter < output_data + data_size; ++iter)
	{
		(*iter) /= float(sqrt(data_size));
	}
	
	return Feed(L"Output", output.get());
} 
