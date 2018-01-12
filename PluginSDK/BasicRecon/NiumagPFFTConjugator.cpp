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
			   int width,
			   int height)
{
	assert(input_data != nullptr && output_data != nullptr && width > 0 && height > 0);
	auto data_size = width * height;

	T* data = new T[data_size];
	memcpy(data, input_data, data_size * sizeof(T));

	if (height < 40)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator>(Conjugate) Input data height must larger than 40!", L"BasicRecon");
		return false;
	}

	auto start = width * (height / 2 + 20);
	for (int i = start; i < data_size; ++i)
	{
		*(output_data + i) = conj(*(data + (data_size - 1 - i)));
	}

	delete []data;
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

	AddProperty<int>(L"DestHeight", 0, L"Destination height.");
}

Yap::NiumagPFFTConjugator::NiumagPFFTConjugator(const NiumagPFFTConjugator& rhs):
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"NiumagPFFTConjugator copy constructor called.", L"BasicRecon");
}

Yap::NiumagPFFTConjugator::~NiumagPFFTConjugator()
{
}

bool Yap::NiumagPFFTConjugator::Input(const wchar_t * name, IData * data)
{
	LOG_TRACE(L"<NiumagPFFTConjugator> Before check.", L"BasicRecon");
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

	auto data_size = input.GetWidth() * input.GetHeight();
	auto dest_height = GetProperty<int>(L"DestHeight");
	if (dest_height == 0)
	{
		dest_height = input.GetHeight();
	}
	if (dest_height < (int)input.GetHeight())
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Improper properties, ReconHeight cannot smaller than input data height!", L"BasicRecon");
		return false;
	}

	auto input_data = GetDataArray<complex<float>>(data);
	if (input_data == nullptr)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Input data is nullptr!", L"BasicRecon");
		return false;
	}

	LOG_TRACE(L"<NiumagPFFTConjugator> After check.", L"BasicRecon");

	// Create output data
	Yap::Dimensions dims;
	dims(DimensionReadout, 0, input.GetWidth())
		(DimensionPhaseEncoding, 0, dest_height);
	auto output = CreateData<complex<float>>(data, &dims);
	auto output_data = GetDataArray<complex<float>>(output.get());
	if (output_data == nullptr)
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Create output data failed!", L"BasicRecon");
		return false;
	}
	memset(output_data, 0, input.GetWidth() * dest_height * sizeof(complex<float>));

	LOG_TRACE(L"<NiumagPFFTConjugator> Create output data.", L"BasicRecon");

	// Get valid data pointer
	auto a = input.GetWidth() * (dest_height - input.GetHeight()) / 2;
	auto valid_data = output_data + input.GetWidth() * (dest_height - input.GetHeight()) / 2;

	// FFT2D
	auto forward_plan = fftwf_plan_dft_2d(int(input.GetHeight()), int(input.GetWidth()),
										(fftwf_complex*)input_data,
										(fftwf_complex*)valid_data,
										FFTW_BACKWARD, FFTW_ESTIMATE);
	fftwf_execute(forward_plan);
	for (auto iter = valid_data; iter < valid_data + data_size; ++iter)
	{
		(*iter) /= float(sqrt(data_size));
	}

	FftShift(valid_data, input.GetWidth(), input.GetHeight());

	LOG_TRACE(L"<NiumagPFFTConjugator> FFT2D.", L"BasicRecon");

	// conjugate
	if (!Conjugate(valid_data, valid_data, input.GetWidth(), input.GetHeight()))
	{
		LOG_ERROR(L"<NiumagPFFTConjugator> Failed when conjugate data!", L"BasicRecon");
		return false;
	}

	LOG_TRACE(L"<NiumagPFFTConjugator> Conjugate.", L"BasicRecon");

	// inverse FFT2D
	auto backward_plan = fftwf_plan_dft_2d(dest_height, int(input.GetWidth()),
										(fftwf_complex*)output_data,
										(fftwf_complex*)output_data,
										FFTW_FORWARD, FFTW_ESTIMATE);
	fftwf_execute(backward_plan);
	for (auto iter = output_data; iter < output_data + dest_height * input.GetWidth(); ++iter)
	{
		(*iter) /= float(sqrt(dest_height * input.GetWidth()));
	}

	LOG_TRACE(L"<NiumagPFFTConjugator> Inverse FFT3D.", L"BasicRecon");

	return Feed(L"Output", output.get());
} 
