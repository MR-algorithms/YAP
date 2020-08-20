#include "Fft2D.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"
#include "time.h"
#include <string>

using namespace std;
using namespace Yap;

Fft2D::Fft2D():
	ProcessorImpl(L"Fft2D"),
	_plan_data_width(0),
	_plan_data_height(0),
	_plan_inverse(false),
	_plan_in_place(false),
	_fft_plan(nullptr)
{
	AddProperty<bool>( L"Inverse", false, L"The direction of FFT2D.");
	AddProperty<bool>( L"InPlace", true, L"The position of FFT2D.");

	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);
}


Fft2D::Fft2D(const Fft2D& rhs)
	:ProcessorImpl(rhs),
	_plan_data_width(rhs._plan_data_width),
	_plan_data_height(rhs._plan_data_height),
	_plan_inverse(rhs._plan_inverse),
	_plan_in_place(rhs._plan_in_place),
	_fft_plan(rhs._fft_plan)
{
}


Fft2D::~Fft2D()
{
}

bool Fft2D::Input(const wchar_t * port, IData * data)
{
	time_t start = clock();

	// Do some check.
	if (data == nullptr)
	{
		LOG_ERROR(L"<Fft2D> Invalid input data!", L"BasicRecon");
		return false;
	}
	if (_wcsicmp(port, L"Input") != 0)
	{
		LOG_ERROR(L"<Fft2D> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper input_data(data);
	if (input_data.GetActualDimensionCount() != 2)
	{
		LOG_ERROR(L"<Fft2D> Error input data dimention!(2D data is available)!", L"BasicRecon");
		return false;
	}
	if (input_data.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<Fft2D> Error input data type!(DataTypeComplexFloat is available)!", L"BasicRecon");
		return false;
	}

	//LOG_TRACE(L"<Fft2D> Input::After Check1.", L"BasicRecon");
	//LOG_TRACE(L"<Fft2D> Input::After Check2.", L"BasicRecon");
	//LOG_TRACE(L"<Fft2D> Input::After Check3.", L"BasicRecon");

	auto width = input_data.GetWidth();
	auto height = input_data.GetHeight();

	auto data_array = GetDataArray<complex<float>>(data);

	if (GetProperty<bool>(L"InPlace"))
	{
		LOG_TRACE(L"<Fft2D> Input::InPlace is true, before Fft().", L"BasicRecon");
		Fft(data_array, data_array, width, height, GetProperty<bool>(L"Inverse"));
		//
		time_t end = clock();
		int ms = float(end - start);// / CLOCKS_PER_SEC;
									//Log
		Log(ms);
		
		return Feed(L"Output", data);
	}
	else
	{
		auto output = CreateData<complex<double>>(data);

		LOG_TRACE(L"<Fft2D> Input::InPlace is false, create data, before Fft().", L"BasicRecon");

		Fft(data_array, GetDataArray<complex<float>>(output.get()),
			width, height, GetProperty<bool>(L"Inverse"));
		//
		time_t end = clock();
		int ms = float(end - start);// / CLOCKS_PER_SEC;
									//Log
		Log(ms);

		//
		return Feed(L"Output", output.get());
	}
}

void Fft2D::FftShift(std::complex<float>* data, size_t  width, size_t height)
{
	LOG_TRACE(L"<Fft2D> FftShift::Before SwapBlock().", L"BasicRecon");
	SwapBlock(data, data + height / 2 * width + width / 2, width / 2, height / 2, width);
	SwapBlock(data + width / 2, data + height / 2 * width, width / 2, height / 2, width);
	LOG_TRACE(L"<Fft2D> FftShift::After SwapBlock().", L"BasicRecon");
}

void Fft2D::SwapBlock(std::complex<float>* block1, std::complex<float>* block2, size_t width, size_t height, size_t line_stride)
{
	std::vector<std::complex<float>> swap_buffer;
	swap_buffer.resize(width);

	auto cursor1 = block1;
	auto cursor2 = block2;
	LOG_TRACE(L"<Fft2D> SwapBlock::Before memcpy().", L"BasicRecon");
	for (unsigned int row = 0; row < height; ++row)
	{
		memcpy(swap_buffer.data(), cursor1, width * sizeof(std::complex<float>));
		memcpy(cursor1, cursor2, width * sizeof(std::complex<float>));
		memcpy(cursor2, swap_buffer.data(), width * sizeof(std::complex<float>));

		cursor1 += line_stride;
		cursor2 += line_stride;
	}
	LOG_TRACE(L"<Fft2D> SwapBlock::After memcpy().", L"BasicRecon");
}

bool Fft2D::Fft(std::complex<float> * data, std::complex<float> * result_data, size_t width, size_t height, bool inverse)
{
	bool in_place = (data == result_data);

	LOG_TRACE(L"<Fft2D> Fft::Before Plan().", L"BasicRecon");
	if (width != _plan_data_width || height != _plan_data_height || inverse != _plan_inverse || in_place != _plan_in_place)
	{
		Plan(width, height, inverse, in_place);
	}
	LOG_TRACE(L"<Fft2D> Fft::After Plan(), before fftwf_execute_dft().", L"BasicRecon");
	fftwf_execute_dft(_fft_plan, (fftwf_complex*)data, (fftwf_complex*)result_data);
	LOG_TRACE(L"<Fft2D> Fft::After fftwf_execute_dft().", L"BasicRecon");

	for (auto data = result_data; data < result_data + width * height; ++data)
	{
		*data /=  float(sqrt(width * height));
	}

	LOG_TRACE(L"<Fft2D> Fft::Before FftShift().", L"BasicRecon");
	FftShift(result_data, width, height);
	LOG_TRACE(L"<Fft2D> Fft::After FftShift().", L"BasicRecon");
	return true;
}

void Fft2D::Plan(size_t width, size_t height, bool inverse, bool in_place)
{
	vector<fftwf_complex> data(width * height);

	if (in_place)
	{
		_fft_plan = fftwf_plan_dft_2d(int(height), int(width), (fftwf_complex*)data.data(),
			(fftwf_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_UNALIGNED);
	}
	else
	{
		vector<fftwf_complex> result(width * height);
		_fft_plan = fftwf_plan_dft_2d(int(height), int(width),  (fftwf_complex*)data.data(),
			(fftwf_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_UNALIGNED);
	}

	_plan_data_width = static_cast<unsigned int> (width);
	_plan_data_height = static_cast<unsigned int> (height);
	_plan_inverse = inverse;
	_plan_in_place = in_place;
}


void Fft2D::Log(float ms)
{
	wstringstream wss;
	wss << L"<Fft2D>: elapsed time =  " << ms << L"ms";
	wstring ws;
	ws = wss.str(); //»ò ss>>strtEST;
	LOG_TRACE(ws.c_str(), L"Fft2D");
}

