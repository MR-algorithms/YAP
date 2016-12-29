#include "stdafx.h"
#include "CompressedSensing.h"
#include "Interface/Client/DataHelper.h"
#include "Interface/Implement/DataObject.h"
#include "vector"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <boost/numeric/ublas/matrix_expression.hpp>

using namespace std;
using namespace Yap;
using namespace boost::numeric::ublas;


CompressedSensing::CompressedSensing():
	ProcessorImpl(L"CompressedSensing"),
	_fft_plan(nullptr),
	_iteration_count(0),
	_coarse_level(0),
	_filter_type_size(0),
	_plan_data_width(0),
	_plan_data_height(0),
	_plan_in_place(false),
	_plan_inverse(false)
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddInput(L"Mask", 2, DataTypeFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);
}


CompressedSensing::~CompressedSensing()
{
}


IProcessor * Yap::CompressedSensing::Clone()
{
	return new (nothrow) CompressedSensing(*this);
}

bool Yap::CompressedSensing::Input(const wchar_t * port, IData * data)
{	
	if (wstring(port) == L"Mask")
	{
		_mask = YapShared(data);
	}
	else if (wstring(port) == L"Input")
	{
		if (!_mask)
			return false;
		DataHelper input_data(data);
		DataHelper input_mask(_mask.get());
		matrix<float> mask(input_mask.GetHeight(), input_mask.GetWidth());
		matrix<complex<float>> undersampled_data(input_data.GetHeight(), input_data.GetWidth());
 		memcpy(&mask(0, 0), GetDataArray<float>(_mask.get()), input_data.GetWidth() * input_data.GetHeight() * sizeof(float));
 		memcpy(&undersampled_data(0, 0), GetDataArray<complex<float>>(data), input_data.GetWidth() * input_data.GetHeight() * sizeof(complex<float>));
		ParametterSet myparameter = { 200, float(pow(10,-30)), 8, 1, 0.002f, 0.005f, 0.01f, 0.8f, 2, mask, undersampled_data};

		auto recon_data = Reconstruct(undersampled_data, myparameter);
		std::complex<float> * recon = nullptr;
		try
		{
			recon = new complex<float>[input_data.GetWidth() * input_data.GetHeight()];
		}
		catch (bad_alloc&)
		{
			return false;
		}
		memcpy(recon, &recon_data(0, 0), input_data.GetWidth() * input_data.GetHeight() * sizeof(complex<float>));
		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, input_data.GetWidth())
			(DimensionPhaseEncoding, 0U, input_data.GetHeight());
		auto output = YapShared(new ComplexFloatData(recon, dimensions, nullptr, true));

		Feed(L"Output", output.get());
	}
	else
	{
		return false;
	}
  return true;
}

matrix<complex<float>> Yap::CompressedSensing::Reconstruct(matrix<complex<float>>& k_space, ParametterSet& myparameter)
{
	_iteration_count = 0;
	float eps = 2.22044604925031f * float(pow(10, -16));

	matrix<complex<float>> g0 = ComputeGradient(k_space, myparameter.wavelet_weight,
		myparameter.tv_weight, myparameter.typenorm, myparameter.mask, myparameter.undersampling_k_space);
	matrix<complex<float>> differential_recon_data = -g0;

	while (1)
	{
		auto recon_image = IFft2DTransform(k_space);
		auto differential_recon_image = IFft2DTransform(differential_recon_data);

		//prepare for calculate fidelity energy；
		matrix<complex<float>> recon_k_data = element_prod(myparameter.mask, k_space);
		matrix<complex<float>> differential_recon_k_data = element_prod(myparameter.mask, differential_recon_data);

		//prepare for calculate wavelet energy；
		auto recon_wavelet_data = Fw2DTransform(recon_image);
		auto differential_recon_wavelet_data = Fw2DTransform(differential_recon_image);

		//prepare for calculate tv energy；
		auto reconstruct_tv_data = TV2DTransform(recon_image);
		auto differential_reconstruct_tv_data = TV2DTransform(differential_recon_image);

		float initial_step = 0.0;
		float initial_energy = CalculateEnergy(recon_k_data,
			differential_recon_k_data,
			recon_wavelet_data,
			differential_recon_wavelet_data,
			reconstruct_tv_data,
			differential_reconstruct_tv_data,
			myparameter, initial_step);
		float step = myparameter.initial_line_search_step;
		float final_energy = CalculateEnergy(recon_k_data,
			differential_recon_k_data,
			recon_wavelet_data,
			differential_recon_wavelet_data,
			reconstruct_tv_data,
			differential_reconstruct_tv_data,
			myparameter, step);
		unsigned int line_search_times = 0;
		auto temp_g0 = conj_multiply(g0 ,differential_recon_data);
		auto sum_temp_g0 = sum(temp_g0);
		auto energy_variation_g0 = abs(sum_temp_g0);

		while ((final_energy > (initial_energy - myparameter.line_search_alpha * step * energy_variation_g0)) &&
			(line_search_times < myparameter.max_line_search_times))
		{
			line_search_times = line_search_times + 1;
			step = step * myparameter.line_search_beta;
			final_energy = CalculateEnergy(recon_k_data,
				differential_recon_k_data,
				recon_wavelet_data,
				differential_recon_wavelet_data,
				reconstruct_tv_data,
				differential_reconstruct_tv_data,
				myparameter, step);
		}
		if (line_search_times == myparameter.max_line_search_times)
		{
			return matrix<complex<float>>();
		}
		if (line_search_times > 2)
		{
			myparameter.initial_line_search_step = myparameter.initial_line_search_step * myparameter.line_search_beta;
		}
		if (line_search_times < 1)
		{
			myparameter.initial_line_search_step = myparameter.initial_line_search_step / myparameter.line_search_beta;
		}

		k_space += differential_recon_data * step;

		matrix<complex<float>> g1 = ComputeGradient(k_space, myparameter.wavelet_weight, myparameter.tv_weight, 
			myparameter.typenorm, myparameter.mask, myparameter.undersampling_k_space);

		auto temp_g1 = square_module(g1);
		auto sum_energy_g1 = sum(temp_g1);
		//auto module_g0 = g0.module();								//
		auto power_module_g0 = square_module(g0);		//
		auto sum_energy_g0 = sum(power_module_g0);		//
		auto ellipse_factor = sum_energy_g1 / (sum_energy_g0 + eps);

		auto g0 = g1;
		differential_recon_data *= ellipse_factor;
		differential_recon_data += matrix<complex<float>>(-g1);
		++_iteration_count;
		auto temp_differential_reconstruct_data = module(differential_recon_data);
		auto differential_reconstruct_data_norm = sum(temp_differential_reconstruct_data);
		if ((_iteration_count > myparameter.max_conjugate_gradient_iteration_times) ||
			(differential_reconstruct_data_norm < myparameter.gradient_tollerance))
		{
			break;
		}
	}
	return k_space;
}

matrix<complex<float>> Yap::CompressedSensing::ComputeGradient(matrix<complex<float>>& in_data, float wavelet_weight, float tv_weight, float p_norm, 
	matrix<float> mask, matrix<complex<float>> subsampled_kdata)
{
	auto gradient = GetFidelityTerm(in_data, mask, subsampled_kdata);
	auto image = IFft2DTransform(in_data);

	if (wavelet_weight)
	{
		auto wavelet_term = GetWaveletTerm(image, p_norm);
		wavelet_term *= wavelet_weight;
		gradient += wavelet_term;
	}

	if (tv_weight)
	{
		auto tv_term = GetTVTerm(image, p_norm);
		tv_term *= tv_weight;
		gradient += tv_term;
	}

	return gradient;
}

matrix<complex<float>> Yap::CompressedSensing::GetFidelityTerm(matrix<complex<float>>& in_data, matrix<float>& mask, matrix<complex<float>> subsampled_kdata)
{
	return (element_prod(mask, in_data) - subsampled_kdata) * 2.0f;
}

matrix<complex<float>> Yap::CompressedSensing::GetWaveletTerm(matrix<complex<float>>& in_data, float p_norm)
{
	auto wavelet_data = Fw2DTransform(in_data);
	float epsilon = static_cast<float>(pow(10, -15));
	auto wavelet_cursor = &wavelet_data(0, 0);
	auto wavelet_cursor_end = &wavelet_data(wavelet_data.size1() - 1, wavelet_data.size2() - 1);
	for (; wavelet_cursor <= wavelet_cursor_end; ++wavelet_cursor)
	{
 		float module_square = norm(*wavelet_cursor);
		*wavelet_cursor *= static_cast<float>(pow(module_square + epsilon, p_norm / 2.0 - 1.0) * p_norm);
	}		
	return Fft2DTransform(IFw2DTransform(wavelet_data));
}

matrix<complex<float>> Yap::CompressedSensing::GetTVTerm(matrix<complex<float>>& in_data, float p_norm)
{
	auto dx = TV2DTransform(in_data);
	float epsilon = static_cast<float>(pow(10, -15));

	for (unsigned int i = 0; i < 2; ++i)
	{
		auto& tv = dx[i];
		auto cursor = &tv(0, 0);
		auto end_cursor = &tv(tv.size1() - 1, tv.size2() - 1);
		for (; cursor <= end_cursor; ++cursor)
		{
 			float module_square = norm(*cursor);
 			*cursor *= static_cast<float>(pow(module_square + epsilon, p_norm / 2.0 - 1.0) * p_norm);
		}		
	}
	return Fft2DTransform(ITV2DTransform(dx));
}



unsigned int Yap::CompressedSensing::QuadLength(unsigned int length)
{
	unsigned int k = 1;
	unsigned int J = 0;
	while (k < length)
	{
		k *= 2;
		J += 1;
	}
	return J;
}

std::vector<float> Yap::CompressedSensing::Iconv(std::vector<float>& filter, std::vector<float>& row)
{
	unsigned int n = row.size();
	unsigned int p = filter.size();
	std::vector<float> xpadded(n + p);
	if (p <= n)
	{
		memcpy(xpadded.data(), row.data() + n - p, p * sizeof(float));
		memcpy(xpadded.data() + p, row.data(), n * sizeof(float));
	}
	else
	{
		std::vector<float> z(p);
		unsigned int imod;
		for (unsigned int i = 0; i < p; ++i)
		{
			imod = 1 + (p * n - p + i) - n * (unsigned)floor((p * n - p + i) / n);
			z[i] = row[imod];
		}
		memcpy(xpadded.data(), z.data(), p * sizeof(float));
		memcpy(xpadded.data() + p, row.data(), n * sizeof(float));
	}
	std::vector<float> ypadded(Filter(filter, 1, xpadded));
	std::vector<float> y(row.size());
	memcpy(y.data(), ypadded.data() + p, n * sizeof(float));
	return y;
}

std::vector<float> Yap::CompressedSensing::Filter(std::vector<float>& filter, unsigned int a, std::vector<float>& in_put)
{
	std::vector<float> temp_filter(filter);
	for (unsigned int i = 0; i < temp_filter.size(); ++i)
	{
		temp_filter.at(i) = temp_filter.at(i) / a;
	}
	std::vector<float> ypadded(in_put.size());
	for (unsigned int i = 0; i < in_put.size(); ++i)
	{
		if (i < temp_filter.size())
		{
			for (unsigned int j = 0; j <= i; ++j)
			{
				ypadded[i] += static_cast<float>(temp_filter.at(j) * in_put[i - j]);
			}
		}
		else
		{
			for (unsigned int j = 0; j < temp_filter.size(); ++j)
			{
				ypadded[i] += static_cast<float>(temp_filter.at(j) * in_put[i - j]);
			}
		}
	}
	return ypadded;
}

std::vector<float> Yap::CompressedSensing::MirrorFilt(std::vector<float>& filter)
{
	std::vector<float> mirror_filter(filter.size());
	for (unsigned int i = 0; i < filter.size(); ++i)
	{
		mirror_filter.at(i) = (-1) * (pow(-1, i + 1)) * filter.at(i);
	}
	return mirror_filter;
}

std::vector<float> Yap::CompressedSensing::Aconv(std::vector<float>& filter, std::vector<float>& row)
{
	auto n = row.size();
	auto p = filter.size();
	std::vector<float> xpadded(row);
	std::vector<float> temp_filter(filter);
	if (p < n)
	{
		xpadded.insert(xpadded.end(), xpadded.begin(), xpadded.begin() + p);
	}
	else
	{
		std::vector<float> z(p);
		unsigned int imod;
		for (unsigned int i = 0; i < p; ++i)
		{
			imod = 1 + i - n * (unsigned)floor(i / n);
			z[i] = row[imod];
		}
		xpadded.insert(xpadded.end(), z.begin(), z.end());
	}

	std::reverse(temp_filter.begin(), temp_filter.end());
	std::vector<float> ypadded(Filter(temp_filter, 1, xpadded));
	std::vector<float> y(row.size());
	memcpy(y.data(), ypadded.data() + p - 1, sizeof(float) * n);
	return y;
}

void Yap::CompressedSensing::SetFilterParams(unsigned int coarse_level)
{
	_coarse_level = coarse_level;
}

void Yap::CompressedSensing::GenerateFilter(wchar_t * filter_type_name, unsigned int coarse_level)
{
	_filter.clear();
	assert(filter_type_name != NULL);
	SetFilterParams(coarse_level);
	
	std::wifstream filter_file;
	filter_file.open(filter_type_name);
	float fdata;
	if (!filter_file)
	{
		return;
	}
	if (filter_file.is_open())
	{
		while (filter_file.good() && !filter_file.eof())
		{
			filter_file >> fdata;
			_filter.push_back(fdata);
		}
	}
	filter_file.close();
}

void Yap::CompressedSensing::SetFilter(float * filter, unsigned int size)
{
	_filter.resize(size);
	for (decltype(_filter.size()) i = 0; i < size; ++i)
	{
		_filter.at(i) = *filter;
		++filter;
	}
}

std::vector<float> Yap::CompressedSensing::DownSamplingLowPass(std::vector<float>& row, std::vector<float>& filter)
{
	std::vector<float> d1(Aconv(filter, row));
	auto n = d1.size();
	std::vector<float> d2;
	for (unsigned int i = 0; i < n - 1; i += 2)
	{
		d2.push_back(d1[i]);	//////////
	}
	return d2;
}

std::vector<float> Yap::CompressedSensing::DownSamplingHighPass(std::vector<float>& row, std::vector<float>& filter)
{
	//MirrorFilt
	std::vector<float> mirror_filter(filter.size());
	mirror_filter = MirrorFilt(filter);
	//Ishift
	std::vector<float> row_lshift(filter.size());
	row_lshift = LeftShift(row);
	std::vector<float> buffer(Iconv(mirror_filter, row_lshift));
	auto n = buffer.size();
	std::vector<float> d;
	for (unsigned int i = 0; i < n - 1; i += 2)
	{
		d.push_back(buffer[i]);
	}
	return d;
}

std::vector<float> Yap::CompressedSensing::LeftShift(std::vector<float>& row)
{
	std::vector<float> row_lshift(row.size());
	memcpy(row_lshift.data(), row.data() + 1, (row.size() - 1) * sizeof(float));
	*(row_lshift.end() - 1) = row.at(0);
	return row_lshift;
}

matrix<float> Yap::CompressedSensing::DownSampling(matrix<float>& output, std::vector<float>& filter, unsigned int nc)
{
	auto width = output.size2();
	auto bot = &output(0, 0);
	auto top = &output(0, nc / 2);
	for (unsigned int ix = 0; ix < nc; ++ix)//行变换
	{		
		std::vector<float> row(nc);//把一行所有的元素提取出来，低通放左边，高通放右边
		memcpy(row.data(), bot + ix * width, nc * sizeof(float));
		memcpy(bot + ix * width, DownSamplingLowPass(row, filter).data(), (nc / 2) * sizeof(float));
		memcpy(top + ix * width, DownSamplingHighPass(row, filter).data(), (nc / 2) * sizeof(float));
	}
	return output;
}


matrix<float> Yap::CompressedSensing::IFWT2D(matrix<float>& input, std::vector<float>& filter, unsigned int scale, ILongTaskListener * listener /*= nullptr*/)
{
	auto width = input.size2();
	auto height = input.size1();
	assert(width == height);

	matrix<float> output(height, width);
	memcpy(&output(0, 0), &input(0, 0), width * height * sizeof(float));
	unsigned int J = QuadLength(width);//a power of 2
	unsigned int nc = static_cast<unsigned int>(pow(2, scale + 1));
	if (listener != nullptr)
	{
		listener->OnBegin();
		listener->OnProgress(0);
	}
	for (unsigned int jscal = 0; jscal < scale; ++jscal)    //  unsigned int jscal = scale; jscal <= J - 1; ++jscal
	{
		//列方向进行反小波变换
		memcpy(&output(0, 0), &Transpose(output)(0, 0), width * height * sizeof(float));
		output = UpSampling(output, filter, nc);
		memcpy(&output(0, 0), &Transpose(output)(0, 0), width * height * sizeof(float));
		output = UpSampling(output, filter, nc);
		nc = nc * 2;
		if (listener != nullptr)
		{
			listener->OnProgress((jscal + 1) * 100 / scale);
		}
	}
	if (listener != nullptr)
	{
		listener->OnEnd();
	}
	return output;
}

matrix<float> Yap::CompressedSensing::UpSampling(matrix<float>& output, std::vector<float>& filter, unsigned int nc)
{
	auto width = output.size2();
	auto top = &output(0, nc / 2);
	auto bot = &output(0, 0);
	for (unsigned int ix = 0; ix < nc; ++ix)				 // 行变换
	{
		std::vector<float> bot_row(nc / 2);
		memcpy(bot_row.data(), bot + ix * width, (nc / 2) * sizeof(float));
		std::vector<float> front(nc);
		memcpy(front.data(), UpSamplingLowPass(bot_row, filter).data(), nc * sizeof(float));
		std::vector<float> top_row(nc / 2);
		memcpy(top_row.data(), top + ix * width, (nc / 2) * sizeof(float));
		std::vector<float> back(nc);
		memcpy(back.data(), UpSamplingHighPass(top_row, filter).data(), nc * sizeof(float));
		std::vector<float> row(nc);
		for (unsigned int i = 0; i < nc; ++i)
		{
			row[i] = front[i] + back[i];
		}
		memcpy(bot, row.data(), nc * sizeof(float));
	}
	return output;
}

std::vector<float> Yap::CompressedSensing::UpSamplingLowPass(std::vector<float>& row, std::vector<float>& filter)
{
	std::vector<float> upsample_row(UpSampleInterpolateZero(row));
	std::vector<float> y;
	y = Iconv(filter, upsample_row);
	return y;
}

std::vector<float> Yap::CompressedSensing::UpSampleInterpolateZero(std::vector<float>& row)
{
	unsigned int s = 2;
	unsigned int n;
	n = row.size() * s;
	std::vector<float> y(n);
	y.assign(y.size(), 0);
	unsigned int j = 0;
	for (unsigned int i = 0; i < n; i += 2)
	{
		y[i] = row[j];
		++j;
	}
	return y;
}

std::vector<float> Yap::CompressedSensing::UpSamplingHighPass(std::vector<float>& row, std::vector<float>& filter)
{
	//MirrorFilt
	std::vector<float> mirror_filter(MirrorFilt(filter));
	//UpSampleN
	std::vector<float> upsample_row(UpSampleInterpolateZero(row));
	//Rshift
	std::vector<float> row_rshift(RightShift(upsample_row));
	std::vector<float> y(Aconv(mirror_filter, row_rshift));
	return y;
}

std::vector<float> Yap::CompressedSensing::RightShift(std::vector<float>& row)
{
	std::vector<float> row_lshift(row.size());
	*(row_lshift.data()) = *(row.end() - 1);
	memcpy(row_lshift.data() + 1, row.data(), (row.size() - 1) * sizeof(float));
	return row_lshift;
}

std::vector<matrix<complex<float>>> Yap::CompressedSensing::TV2DTransform(matrix<complex<float>>& in_data)
{
	size_t width = in_data.size2();
	size_t height = in_data.size1();

	std::vector<matrix<complex<float>>> tv_result(2);
	tv_result[0].resize(height, width);
	tv_result[1].resize(height, width);
	auto& vertical_tv = tv_result[0];

	for (size_t i = 0; i < height - 1; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			vertical_tv(i, j) = in_data(i + 1, j) - in_data(i ,j);
		}
	}
	auto& horizontal_tv = tv_result[1];
	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width - 1; ++j)
		{
			horizontal_tv(i, j) = in_data(i, j + 1) - in_data(i, j);
		}
	}
	return tv_result;
}

matrix<complex<float>> Yap::CompressedSensing::ITV2DTransform(std::vector<matrix<complex<float>>>& in_data)
{
	size_t width = in_data[0].size2();
	size_t height = in_data[0].size1();

	matrix<complex<float>> itv_result(height, width);
	matrix<complex<float>> temp_x(height, width);
	matrix<complex<float>> temp_y(height, width);

	auto& vertical_tv = in_data[0];
	for (size_t i = 0; i < height - 1; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			if (i < 1)
				temp_x(i, j) = vertical_tv(i, j) * (-1.0f);
			else
				temp_x(i, j) = vertical_tv(i - 1, j) - vertical_tv(i, j);
		}
	}
	for (size_t j = 0; j < width; ++j)
	{
		temp_x(height - 1, j) = vertical_tv(height - 2, j);
	}

	auto& horizontal_tv = in_data[1];
	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			if (j != 0)
			{
				temp_y(i, j) = horizontal_tv(i, j - 1) - horizontal_tv(i, j);
			}
			else
			{
				temp_y(i, j) = horizontal_tv(i, j) * (-1.0f);
				if (i != 0 )
				{
					temp_y(i - 1, width - 1) = horizontal_tv(i - 1, width - 2);
				}
			}
			temp_y(height - 1, width - 1) = horizontal_tv(height - 1, width - 2);
		}
	}
	itv_result = temp_x + temp_y;

	return itv_result;
}

matrix<complex<float>> Yap::CompressedSensing::Fft2DTransform(matrix<complex<float>> in_data)
{
	matrix<complex<float>> image_slice;
	image_slice.resize(in_data.size1(), in_data.size2());
	FftShift(in_data);  // 由于小波变换问题，必须把k-space先fftshift
	Plan(image_slice.size2(), image_slice.size1(), false, false);
//	complex<float>* p_input = &in_data(0, 0);
//	complex<float>* p_image = &image_slice(0, 0);

		//从图像到k空间是傅里叶变换
		fftwf_execute_dft(_fft_plan, (fftwf_complex*)&in_data(0, 0), (fftwf_complex*)&image_slice(0, 0));
		FftShift(image_slice);

	image_slice /= sqrt(in_data.size1() * in_data.size2());

	return image_slice;
}

matrix<complex<float>> Yap::CompressedSensing::IFft2DTransform(matrix<complex<float>> in_data)
{
	matrix<complex<float>> image_slice;
	image_slice.resize(in_data.size1(), in_data.size2());
	FftShift(in_data);  // 由于小波变换问题，必须把k-space先fftshift
	Plan(image_slice.size2(), image_slice.size1(), true, false);
// 	complex<float>* p_in_data = &in_data(0, 0);
// 	complex<float>* p_image_slice = &image_slice(0, 0);

	//从k空间到图像是反傅里叶变换
	fftwf_execute_dft(_fft_plan, (fftwf_complex*)&in_data(0, 0), (fftwf_complex*)&image_slice(0, 0));
	FftShift(image_slice);

	image_slice /= sqrt(in_data.size1() * in_data.size2());

	return image_slice;
}


void Yap::CompressedSensing::FftShift(matrix<complex<float>>& data)
{
	auto width = data.size2();
	auto height = data.size1();
	SwapBlock(&data(0, 0), &data(0, 0) + height / 2 * width + width / 2, width / 2, height / 2, width);
	SwapBlock(&data(0, 0) + width / 2, &data(0, 0) + height / 2 * width, width / 2, height / 2, width);
}

void Yap::CompressedSensing::SwapBlock(std::complex<float> * block1, std::complex<float> * block2, 
	size_t width, size_t height, size_t line_stride)
{
	std::vector<std::complex<float>> swap_buffer;
	swap_buffer.resize(width);

	auto cursor1 = block1;
	auto cursor2 = block2;
	for (size_t row = 0; row < height; ++row)
	{
		memcpy(swap_buffer.data(), cursor1, width * sizeof(std::complex<float>));
		memcpy(cursor1, cursor2, width * sizeof(std::complex<float>));
		memcpy(cursor2, swap_buffer.data(), width * sizeof(std::complex<float>));

		cursor1 += line_stride;
		cursor2 += line_stride;
	}
}

matrix<float> Yap::CompressedSensing::FWT2D(matrix<float>& input, std::vector<float>& filter, unsigned int scale, ILongTaskListener * listener)
{
	auto width = input.size2();
	auto height = input.size1();
	assert(width == height);
	matrix<float> output(height, width);
	memcpy(&output(0, 0), &input(0, 0), width * height * sizeof(float));

	unsigned int J = QuadLength(width);//a _iwavelet_slice_index of 2
	unsigned int nc = width;

	assert(J >= scale);
	if (listener != nullptr)
	{
		listener->OnBegin();
		listener->OnProgress(0);
	}
	for (unsigned int jscal = 0; jscal < scale; ++jscal)     // unsigned int jscal = J - 1; jscal >= scale; --jscal
	{
		output = DownSampling(output, filter, nc);// 行方向下采样
		memcpy(&output(0, 0), &Transpose(output)(0, 0), width * height * sizeof(float));
		output = DownSampling(output, filter, nc);//列方向下采样
		memcpy(&output(0, 0), &Transpose(output)(0, 0), width * height * sizeof(float));
		nc = nc / 2;
		if (listener != nullptr)
		{
			listener->OnProgress((jscal + 1) * 100 / scale);
		}
	}
	if (listener != nullptr)
	{
		listener->OnEnd();
	}
	return output;
}

matrix<complex<float>> Yap::CompressedSensing::Fw2DTransform(matrix<complex<float>>& input, ILongTaskListener * listener)
{
//	int t = sizeof(int);	//
	matrix<complex<float>>& in_data(input);
	auto width = in_data.size2();
	auto height = in_data.size1();
	matrix<float> in_real_part(height, width);
	matrix<float> in_imaginary_part(height, width);
	matrix<complex<float>> wave_data;
	wave_data.resize(height, width);
	//提取实部、虚部
//	in_real_part = real(in_data);
//	in_imaginary_part = imag(in_data);
	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			in_real_part(i, j) = in_data(i, j).real();
			in_imaginary_part(i, j) = in_data(i, j).imag();
		}
	}
	
	//CGenerateFilter filter(_T("db"), 3, 4); //第二个参数决定基的类型，第三个参数决定小波级数
	GenerateFilter(L"daub2.flt", 4);
	//小波变换
	auto output_real_part = FWT2D(in_real_part, _filter, _coarse_level, listener);
	auto output_imaginary_part = FWT2D(in_imaginary_part, _filter, _coarse_level, listener);
	//组合复数
	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			wave_data(i, j) = std::complex<float>(output_real_part(i, j), output_imaginary_part(i, j));
		}
	}

	return wave_data;
}

matrix<complex<float>> Yap::CompressedSensing::IFw2DTransform(matrix<complex<float>>& input, ILongTaskListener * listener /*= nullptr*/)
{
	matrix<complex<float>>& in_data(input);
	auto width = in_data.size2();
	auto height = in_data.size1();
	matrix<float> in_real_part(height, width);
	matrix<float> in_imaginary_part(height, width);
	matrix<complex<float>> iwave_data;
	iwave_data.resize(height, width);

	//提取实部、虚部
	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			in_real_part(i, j) = in_data(i, j).real();
			in_imaginary_part(i, j) = in_data(i, j).imag();
		}
	}

	GenerateFilter(L"daub2.flt", 4);
	//反小波变换
	auto output_real_part = IFWT2D(in_real_part, _filter, _coarse_level, listener);
	auto output_imaginary_part = IFWT2D(in_imaginary_part, _filter, _coarse_level, listener);
	//组合复数
	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			iwave_data(i, j) = std::complex<float>(output_real_part(i, j), output_imaginary_part(i, j));
		}
	}
	return iwave_data;
}

float Yap::CompressedSensing::CalculateEnergy(matrix<complex<float>>& recon_k_data, 
	                                          matrix<complex<float>>& differential_recon_k_data, 
	                                          matrix<complex<float>>& recon_wavelet_data, 
	                                          matrix<complex<float>>& differential_recon_wavelet_data, 
	                                          std::vector<matrix<complex<float>>>& recon_tv_data, 
	                                          std::vector<matrix<complex<float>>>& differential_recon_tv_data, 
	                                          ParametterSet& myparameter, 
	                                          float step_length)
{
	float total_energy = 0.0;
	float fidelity_energy = 0.0;
	matrix<complex<float>> temp_fidelity_energy = recon_k_data + differential_recon_k_data * step_length - myparameter.undersampling_k_space;

	auto power_module_temp_fidelity_energy = square_module(temp_fidelity_energy);
	fidelity_energy = sum(power_module_temp_fidelity_energy);
	float epsilon = static_cast<float>(pow(10, -15));
	matrix<float> epsilon_m(recon_wavelet_data.size1(), recon_wavelet_data.size2());

	float wavelet_energy = 0.0;
	if (myparameter.wavelet_weight)
	{
		matrix<complex<float>> temp_wavelet_energy = recon_wavelet_data + differential_recon_wavelet_data * step_length;
		//auto module_wavelet_energy = temp_wavelet_energy.module();
		matrix<float> power_module_temp_wavelet_energy = square_module(temp_wavelet_energy) + fill(epsilon, epsilon_m);  //模的平方
		//auto temp = DataType::power(power_module_temp_wavelet_energy, p_norm/2);
		//数学上等同于上式：p_norm = 1；
		auto temp = sqrt_root(power_module_temp_wavelet_energy);
		wavelet_energy = sum(temp) * myparameter.wavelet_weight;
	}

	float tv_energy = 0.0;
	if (myparameter.tv_weight)
	{
		for (unsigned int i = 0; i < 2; ++i)
		{
			matrix<complex<float>> temp_tv_energy = recon_tv_data[i] + differential_recon_tv_data[i] * step_length;
			//auto module_tv_energy = temp_tv_energy.module();
			matrix<float> power_module_temp_tv_energy = square_module(temp_tv_energy) + fill(epsilon, epsilon_m);			//模的平方
		    //auto temp = DataType::power(power_module_temp_tv_energy, p_norm/2);
			//数学意义上等同于上式：p_norm = 1
			auto temp = sqrt_root(power_module_temp_tv_energy);
			tv_energy += sum(temp) * myparameter.tv_weight;
		}
	}

	total_energy = fidelity_energy + wavelet_energy + tv_energy;
	return total_energy;
}


boost::numeric::ublas::matrix<float> Yap::CompressedSensing::Transpose(boost::numeric::ublas::matrix<float>& in_data)
{
	matrix<float> output(in_data.size2(), in_data.size1());

	for (size_t i = 0; i < in_data.size1(); ++i)
	{
		for (size_t j = 0; j < in_data.size2(); ++j)
		{
			output(j, i) = in_data(i, j);
		}
	}
	return output;
}

matrix<float> Yap::CompressedSensing::square_module(matrix<complex<float>>& input)
{
	matrix<float> normal(input.size1(), input.size2());
	for (size_t i = 0; i < input.size1(); ++i)
	{
		for (size_t j = 0; j < input.size2(); ++j)
		{
			normal(i, j) = norm(input(i, j));
		}
	}
	return normal;
}

matrix<float> Yap::CompressedSensing::fill(float value, matrix<float>& input)
{
	for (size_t i = 0; i < input.size1(); ++i)
	{
		for (size_t j = 0; j < input.size2(); ++j)
		{
			input(i, j) = value;
		}
	}
	return input;
}

matrix<float> Yap::CompressedSensing::sqrt_root(matrix<float> input)
{
	matrix<float> output(input.size1(), input.size2());
	for (size_t i = 0; i < input.size1(); ++i)
	{
		for (size_t j = 0; j < input.size2(); ++j)
		{
			output(i, j) = sqrt(input(i, j));
		}
	}
	return output;
}


matrix<complex<float>> Yap::CompressedSensing::conj_multiply(matrix<complex<float>>& input_1, matrix<complex<float>>& input_2)
{
	matrix<complex<float>> result(input_1.size1(), input_1.size2());
	for (size_t i = 0; i < input_1.size1(); ++i)
	{
		for (size_t j = 0; j < input_1.size2(); ++j)
		{
			result(i, j) = conj(input_1(i, j)) * input_2(i, j);
		}
	}
	return result;
}

matrix<float> Yap::CompressedSensing::module(matrix<complex<float>> input)
{
	matrix<float> result(input.size1(), input.size2());
	for (size_t i = 0; i < input.size1(); ++i)
	{
		for (size_t j = 0; j < input.size2(); ++j)
		{
			result(i, j) = abs(input(i, j));
		}
	}
	return result;
}

void Yap::CompressedSensing::Plan(size_t width, size_t height, bool inverse, bool inplace)
{
	std::vector<fftwf_complex> data(width * height);

	if (inplace)
	{
		_fft_plan = fftwf_plan_dft_2d(int(width), int(height), (fftwf_complex*)data.data(),
			(fftwf_complex*)data.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}
	else
	{
		std::vector<fftwf_complex> result(width * height);
		_fft_plan = fftwf_plan_dft_2d(int(width), int(height), (fftwf_complex*)data.data(),
			(fftwf_complex*)result.data(),
			inverse ? FFTW_BACKWARD : FFTW_FORWARD,
			FFTW_MEASURE);
	}
	_plan_data_width = static_cast<unsigned int> (width);
	_plan_data_height = static_cast<unsigned int> (height);
	_plan_inverse = inverse;
	_plan_in_place = inplace;
}

float Yap::CompressedSensing::sum(boost::numeric::ublas::matrix<float>& input)
{
	float sum_value = 0;
	for (unsigned int i = 0; i < input.size1(); ++i)
	{
		for (unsigned int j = 0; j < input.size2(); ++j)
		{
			sum_value += input(i, j);
		}
	}
	return sum_value;
}

std::complex<float> Yap::CompressedSensing::sum(boost::numeric::ublas::matrix<std::complex<float>>& input)
{
	complex<float> sum_value = 0;
	for (unsigned int i = 0; i < input.size1(); ++i)
	{
		for (unsigned int j = 0; j < input.size2(); ++j)
		{
			sum_value += input(i, j);
		}
	}
	return sum_value;
}

