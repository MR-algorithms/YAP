#include "stdafx.h"
#include "SamplingTypeGenerator.h"
#include <math.h>
#include "DataHelper.h"
#include <algorithm>
#include <numeric>
#include "../FFTW-64/include/fftw3.h"

#pragma comment(lib, "../FFTW-64/lib/libfftw3-3.lib")
#pragma comment(lib, "../FFTW-64/lib/libfftw3f-3.lib")
#pragma comment(lib, "../FFTW-64/lib/libfftw3l-3.lib")


using namespace Yap;
using namespace std;

CSamplingTypeGenerator::CSamplingTypeGenerator(void):
	CProcessorImp(L"SamplingTypeGenerator"),
	_try_count(10),
	_tolerance(3)
{
	AddInputPort(L"Input", 0, DataTypeUnknown);
	AddOutputPort(L"Output", 1, DataTypeFloat);

	AddProperty(PropertyFloat, L"pow", L"");
	SetFloat(L"pow", 3.0f);
	AddProperty(PropertyFloat, L"sample_percent", L"");
	SetFloat(L"sample_percent", 0.4);
	AddProperty(PropertyFloat, L"radius", L"");
	SetFloat(L"radius", 0.2f);
	AddProperty(PropertyBool, L"equal_unsampling", L"");
	SetBool(L"equal_unsampling", false);
	AddProperty(PropertyBool, L"random_unsampling", L"");
	SetBool(L"random_unsampling", true);
}

Yap::CSamplingTypeGenerator::CSamplingTypeGenerator(const CSamplingTypeGenerator & rhs)
	:CProcessorImp(rhs)
{	
}


CSamplingTypeGenerator::~CSamplingTypeGenerator()
{
}

IProcessor * Yap::CSamplingTypeGenerator::Clone()
{
	try
	{
		auto processor = new CSamplingTypeGenerator(*this);
		return processor;
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

bool Yap::CSamplingTypeGenerator::Input(const wchar_t * name, IData * data)
{
	if (wstring(name) != L"Input")
		return false;

	if (data->GetDataType() != DataTypeComplexFloat)
		return false;

	CDataHelper input_data(data);
	if (GetBool(L"random_unsampling"))
	{
		auto row_count = input_data.GetHeight();
		float pow = static_cast<float> (GetFloat(L"pow"));
		float sample_percent = static_cast<float> (GetFloat(L"sample_percent"));
		float radius = static_cast<float> (GetFloat(L"radius"));

		auto sampling_pattern = GetMinInterferenceSamplingPattern(row_count, pow, sample_percent, radius);
		char * sampling_type = nullptr;
		try
		{
			sampling_type = new char[row_count];
		}
		catch(bad_alloc&)
		{
			return false;
		}
		memcpy(sampling_type, sampling_pattern.data(), sizeof(char));

		CDimensionsImp dimensions;
		dimensions(DimensionReadout, 0U, 1)
			(DimensionPhaseEncoding, 0U, row_count);
		CSmartPtr<CCharData> outdata(new CCharData(sampling_type, dimensions, nullptr, true));

		Feed(L"Output", outdata.get());
	}
	else
	{

	}
	

	return true;
}

std::vector<unsigned char> Yap::CSamplingTypeGenerator::GetMinInterferenceSamplingPattern(unsigned int row_count, float pow, float sample_percent, float radius)
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

std::vector<float> Yap::CSamplingTypeGenerator::GeneratePdf(unsigned int row_count, float p, float sample_percent, float radius)
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

std::vector<float> Yap::CSamplingTypeGenerator::LineSpace(float begin, float end, unsigned int count)
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
