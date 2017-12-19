#include "SamplingMaskCreator.h"
#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <math.h>
#include <numeric>
#include <algorithm>


using namespace Yap;
using namespace std;

SamplingMaskCreator::SamplingMaskCreator():
	ProcessorImpl(L"SamplingMaskCreator"),
	_try_count(10),
	_tolerance(3)
{
	LOG_TRACE(L"SamplingMaskCreator constructor called.", L"BasicRecon");
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeFloat);

	AddProperty<double>(L"Pow", 3.0, L"");
	AddProperty<double>(L"SamplePercent", 0.4, L"");
	AddProperty<double>(L"Radius", 0.2, L"");

	AddProperty<bool>(L"EqualSubsampling", true, L"");
	AddProperty<bool>(L"RandomSubsampling", false, L"");
	
	AddProperty<int>(L"Rate", 2, L"");
	AddProperty<int>(L"AcsCount", 16, L"");
}

SamplingMaskCreator::SamplingMaskCreator(const SamplingMaskCreator& rhs)
	:ProcessorImpl(rhs)
{
	LOG_TRACE(L"SampleingMaskCreator constructor called.", L"BasicRecon");
}
SamplingMaskCreator::~SamplingMaskCreator()
{
}

bool Yap::SamplingMaskCreator::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	if (data->GetDataType() != DataTypeComplexFloat)
		return false;

	DataHelper input_data(data);
	if (GetProperty<bool>(L"RandomSubsampling"))
	{
		auto height = input_data.GetHeight();
		auto width = input_data.GetWidth();
		double pow = GetProperty<double>(L"Pow");
		double sample_percent = GetProperty<double>(L"SamplePercent");
		double radius = GetProperty<double>(L"Radius");

		auto mask = GenerateRandomMask(width, height, (float)pow, (float)sample_percent, (float)radius);
		float * mask_buffer = nullptr;
		try
		{
			mask_buffer = new float[width * height];
		}
		catch(bad_alloc&)
		{
			return false;
		}
		memcpy(mask_buffer, mask.data(), width * height * sizeof(float));

		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, width)
			(DimensionPhaseEncoding, 0U, height)
			(DimensionSlice, 0U, 1)
			(Dimension4, 0U, 1)
			(DimensionChannel, 0U, 1);

		auto outdata = CreateData<float>(data, mask_buffer, dimensions, nullptr, true);

		Feed(L"Output", outdata.get());
	}
	else
	{
		unsigned int r = GetProperty<int>(L"Rate");
		unsigned int acs = GetProperty<int>(L"AcsCount");
		auto height = input_data.GetHeight();
		auto width = input_data.GetWidth();
		auto mask = GenerateEqualMask(width, height, acs, r);
		float * mask_buffer = nullptr;
		try
		{
			mask_buffer = new float[width * height];
		}
		catch (bad_alloc&)
		{
			return false;
		}
		memcpy(mask_buffer, mask.data(), width * height * sizeof(float));

		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, width)
			(DimensionPhaseEncoding, 0U, height)
			(DimensionSlice, 0U, 1)
			(Dimension4, 0U, 1)
			(DimensionChannel, 0U, 1);

		auto outdata = CreateData<float>(data, mask_buffer, dimensions, nullptr, true);

		Feed(L"Output", outdata.get());
	}
	return true;
}

std::vector<float> Yap::SamplingMaskCreator::GenerateRandomMask(unsigned int width, unsigned int height, 
	float pow, float sample_percent, float radius)
{
	std::vector<unsigned int> sampling_pattern = GetRandomSamplingPattern(height, pow, sample_percent, radius);
	std::vector<float> mask(width * height);
	 auto mask_cursor = mask.data();

	 for (unsigned int row = 0; row < height; ++row)
	 {
		 for (unsigned int column = 0; column < width; ++column)
		 {
			 *mask_cursor++ = float(sampling_pattern[row]);
		 }
	 }
	return mask;
}

std::vector<float> Yap::SamplingMaskCreator::GenerateEqualMask(
	unsigned int width, 
	unsigned int height, 
	unsigned int acs, 
	unsigned int rate)
{
	std::vector<unsigned int> sampling_pattern = GetEqualSamplingPattern(height, acs, rate);
	std::vector<float> mask(width * height);
	auto mask_cursor = mask.data();

	for (unsigned int row = 0; row < height; ++row)
	{
		for (unsigned int column = 0; column < width; ++column)
		{
			*mask_cursor++ = float(sampling_pattern[row]);
		}
	}
	return mask;
}

std::vector<unsigned int> Yap::SamplingMaskCreator::GetRandomSamplingPattern(
	unsigned int row_count, 
	float pow, 
	float sample_percent, 
	float radius)
{
	float min_peak_interference((float)INT_MAX);
	vector<float> pdf = GeneratePdf(row_count, pow, sample_percent, radius);

	vector<unsigned int> min_interference_pattern(pdf.size());
	vector<unsigned int> sampling_pattern(pdf.size());
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

std::vector<unsigned int> Yap::SamplingMaskCreator::GetEqualSamplingPattern(
	unsigned int height, 
	unsigned int acs, 
	unsigned int rate)
{
	std::vector<unsigned int> sampling_pattern(height, 0);
	for (unsigned int i = 0; i < height; ++i)
	{
		if (i % rate == 0)
		{
			sampling_pattern[i] = 1;
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

std::vector<float> Yap::SamplingMaskCreator::GeneratePdf(
	unsigned int row_count, 
	float p, 
	float sample_percent,
	float radius)
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

	if (floor(sum) > line_count)
	{
		return vector<float>();  // p值偏小。
	}

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

