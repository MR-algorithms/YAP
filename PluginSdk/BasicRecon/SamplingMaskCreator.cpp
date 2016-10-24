
#include "SamplingMaskCreator.h"
#include "Interface/Client/DataHelper.h"

#include <math.h>
#include <algorithm>
#include <numeric>
#include <fftw3.h>

#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")


using namespace Yap;
using namespace std;

SamplingMaskCreator::SamplingMaskCreator(void):
	ProcessorImpl(L"SamplingMaskCreator"),
	_try_count(10),
	_tolerance(3)
{
}

Yap::SamplingMaskCreator::SamplingMaskCreator(const SamplingMaskCreator & rhs)
	:ProcessorImpl(rhs)
{	
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"Mask", 2, DataTypeChar);

	AddProperty(PropertyFloat, L"pow", L"");
	SetFloat(L"pow", 3.0f);
	AddProperty(PropertyFloat, L"sample_percent", L"");
	SetFloat(L"sample_percent", 0.4f);
	AddProperty(PropertyFloat, L"radius", L"");
	SetFloat(L"radius", 0.2f);

	AddProperty(PropertyBool, L"equal_subsampling", L"");
	SetBool(L"equal_subsampling", true);
	AddProperty(PropertyBool, L"random_subsampling", L"");
	SetBool(L"random_subsampling", false);

	AddProperty(PropertyInt, L"Rate", L"");
	SetInt(L"Rate", 2);
	AddProperty(PropertyInt, L"AcsCount", L"");
	SetInt(L"AcsCount", 16);
}


SamplingMaskCreator::~SamplingMaskCreator()
{
}

IProcessor * Yap::SamplingMaskCreator::Clone()
{
	return new (nothrow) SamplingMaskCreator(*this);
}

bool Yap::SamplingMaskCreator::Input(const wchar_t * name, IData * data)
{
	if (wstring(name) != L"Input")
		return false;

	if (data->GetDataType() != DataTypeComplexFloat)
		return false;

	DataHelper input_data(data);
	if (GetBool(L"random_subsampling"))
	{
		auto height = input_data.GetHeight();
		auto width = input_data.GetWidth();
		float pow = static_cast<float> (GetFloat(L"pow"));
		float sample_percent = static_cast<float> (GetFloat(L"sample_percent"));
		float radius = static_cast<float> (GetFloat(L"radius"));

		auto mask = GenerateRandomMask(width, height, pow, sample_percent, radius);
		char * Mask = nullptr;
		try
		{
			Mask = new char[width * height];
		}
		catch(bad_alloc&)
		{
			return false;
		}
		memcpy(Mask, mask.data(), width * height * sizeof(char));

		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, width)
			(DimensionPhaseEncoding, 0U, height);

		auto outdata = YapShared(new CharData(Mask, dimensions, nullptr, true));

		Feed(L"Mask", outdata.get());
	}
	else
	{
		unsigned int r = GetInt(L"Rate");
		unsigned int acs = GetInt(L"AcsCount");
		auto height = input_data.GetHeight();
		auto width = input_data.GetWidth();
		auto mask = GenerateEqualMask(width, height, acs, r);
		char * Mask = nullptr;
		try
		{
			Mask = new char[width * height];
		}
		catch (bad_alloc&)
		{
			return false;
		}
		memcpy(Mask, mask.data(), width * height * sizeof(char));

		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, width)
			(DimensionPhaseEncoding, 0U, height);

		auto outdata = YapShared(new CharData(Mask, dimensions, nullptr, true));

		Feed(L"Mask", outdata.get());
	}
	return true;
}

std::vector<unsigned char> Yap::SamplingMaskCreator::GenerateRandomMask(unsigned int width, unsigned int height, float pow, float sample_percent, float radius)
{
	std::vector<unsigned char> sampling_pattern = GetRandomSamplingPattern(height, pow, sample_percent, radius);
	std::vector<unsigned char> mask(width * height);
	 auto mask_cursor = mask.data();

	 for (unsigned int row = 0; row < height; ++row)
	 {
		 for (unsigned int column = 0; column < width; ++column)
		 {
			 *mask_cursor++ = sampling_pattern[width * row + column];
		 }
	 }
	return mask;
}

std::vector<unsigned char> Yap::SamplingMaskCreator::GenerateEqualMask(unsigned int width, unsigned int height, unsigned int acs, unsigned int rate)
{
	std::vector<unsigned char> sampling_pattern = GetEqualSamplingPattern(height, acs, rate);
	std::vector<unsigned char> mask(width * height);
	auto mask_cursor = mask.data();

	for (unsigned int row = 0; row < height; ++row)
	{
		for (unsigned int column = 0; column < width; ++column)
		{
			*mask_cursor++ = sampling_pattern[width * row + column];
		}
	}
	return mask;
}

std::vector<unsigned char> Yap::SamplingMaskCreator::GetRandomSamplingPattern(unsigned int row_count, float pow, float sample_percent, float radius)
{
	float min_peak_interference((float)INT_MAX);
	vector<float> pdf = GeneratePdf(row_count, pow, sample_percent, radius);

	vector<unsigned char> min_interference_pattern(pdf.size());
	vector<unsigned char> sampling_pattern(pdf.size());
	vector<complex<float>> normalized_sampling_pattern(pdf.size());
	vector<complex<float>> fft_result(pdf.size());

	fftwf_plan p = fftwf_plan_dft_1d(int(normalized_sampling_pattern.size()),
		(fftwf_complex*)normalized_sampling_pattern.data(),
		(fftwf_complex*)fft_result.data(),
		FFTW_BACKWARD, FFTW_ESTIMATE);	//从k空间到图像是反傅里叶变换

	for (unsigned int i = 0; i < _try_count; ++i)
	{
		float sum_vector_element = 0;

		float sum_pdf = accumulate(pdf.begin(), pdf.end(), 0.0f);
		while (abs(sum_vector_element - sum_pdf) > _tolerance)
		{
			sum_vector_element = 0;
			for (unsigned int j = 0; j < pdf.size(); ++j)
			{
				sampling_pattern[j] = ((float)rand() / (RAND_MAX + 1) * 1) < pdf[j];
				sum_vector_element += sampling_pattern[j];
			}
		}

		//转换为复数形式

		for (unsigned int t = 0; t < pdf.size(); t++)
		{
			normalized_sampling_pattern[t] = std::complex<float>(sampling_pattern[t] / pdf[t], 0);
		}
		// 傅里叶变换

		fftwf_execute(p);
		for (auto iter = fft_result.begin() + 1; iter != fft_result.end(); ++iter)
		{
			(*iter) /= float(sqrt(fft_result.size()));
		}
		*(fft_result.data()) = 0;
		float max_value = abs(fft_result[1]);
		for (auto it = fft_result.begin() + 1; it != fft_result.end(); ++it)
		{

			if (max_value < abs(*it))
			{
				max_value = abs(*it);
			}
		}

		if (max_value < min_peak_interference)
		{
			min_peak_interference = max_value;
			min_interference_pattern = sampling_pattern;
		}
	}
	//
	fftwf_destroy_plan(p);

	return min_interference_pattern;
}

std::vector<unsigned char> Yap::SamplingMaskCreator::GetEqualSamplingPattern(unsigned int height, unsigned int acs, unsigned int rate)
{
	std::vector<unsigned char> sampling_pattern;
	for (unsigned int i = 0; i <= height - rate - 1; ++i)
	{
		if (i % rate == 0)
		{
			sampling_pattern.push_back(1);
		}
		else
		{
			sampling_pattern.push_back(0);
		}
	}

	unsigned int first = static_cast<unsigned int>((floor((height - acs) / (2 * rate))) * rate + 1);
	unsigned int last = first + acs;
	sampling_pattern[first] = 1;
	while (first <= last)
	{
		++first;
		sampling_pattern[first] = 1;
	}
	return sampling_pattern;
}

std::vector<float> Yap::SamplingMaskCreator::GeneratePdf(unsigned int row_count, float p, float sample_percent, float radius)
{
	float minval = 0.0;
	float maxval = 1.0;
	float val = 0.5;
	unsigned int line_count = (unsigned int)floor(sample_percent * row_count);    // PCTG = 0.35 * 512 in Matlab
	float gap = float(2.0 / (row_count - 1));

	std::vector<float> pdf;

	pdf = LineSpace(-1, 1, row_count);
	std::vector<unsigned int> pdf_index(row_count);
	for (unsigned int i = 0; i < pdf.size(); ++i)
	{
		pdf_index[i] = 0;
		if (abs(pdf[i]) < radius)
		{
			pdf_index[i] = i;
		}
	}

	for (unsigned int i = 0; i < pdf.size(); ++i)
	{
		pdf[i] = pow(1 - abs(pdf[i]), p);
		if (i == pdf_index[i])
		{
			pdf[i] = 1;
		}
	}

	float sum = accumulate(pdf.begin(), pdf.end(), 0.0f);


	std::vector<float> pdf_temp;
	while (1)
	{
		val = minval / 2 + maxval / 2;
		pdf_temp.erase(pdf_temp.begin(), pdf_temp.end());
		for (unsigned int i = 0; i < row_count; ++i)
		{
			float pdf_elem = pdf[i] + val;
			if (pdf_elem > 1.0)
			{
				pdf_elem = 1.0;
			}
			pdf_temp.push_back(pdf_elem);
		}

		sum = floor(accumulate(pdf_temp.begin(), pdf_temp.end(), 0.0f));

		if (floor(sum) > line_count)
		{
			return vector<float>();  //p值偏小。
		}

		if (sum > line_count)
		{
			maxval = val;
		}
		else if (sum < line_count)
		{
			minval = val;
		}
		else
			break;
	}
	return pdf_temp;
}

std::vector<float> Yap::SamplingMaskCreator::LineSpace(float begin, float end, unsigned int count)
{
	std::vector<float> vec;
	float current_value = begin;
	float gap = float(2.0 / (count - 1));

	while (current_value <= end)
	{
		vec.push_back(current_value);
		current_value += gap;
	}
	while (vec.size() < count)
	{
		vec.push_back(end);
	}

	return vec;
}


