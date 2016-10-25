#include "stdafx.h"
#include "CompressedSensing.h"
#include "Interface/Client/DataHelper.h"


using namespace std;
using namespace Yap;
using namespace arma;

CompressedSensing::CompressedSensing(bool fft_shift):
	ProcessorImpl(L"CompressedSensing"),
	_fft_shift(fft_shift),
	_p_FFT(nullptr),
	_p_IFFT(nullptr),
	_iteration_count(0),
	_coarse_level(0),
	_filter_type_size(0)
{
}


CompressedSensing::~CompressedSensing(void)
{
}

bool Yap::CompressedSensing::OnInit()
{
	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddInput(L"Mask", 2, DataTypeChar);
	AddOutput(L"Output", 2, DataTypeComplexFloat);
	return true;
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
		fmat Mask(input_mask.GetWidth(), input_mask.GetHeight());
		cx_fmat Undersampled_data(input_data.GetWidth(), input_data.GetHeight());
		
		memcpy(Mask.memptr(), GetDataArray<float>(_mask.get()), input_mask.GetWidth() * input_mask.GetHeight() * sizeof(float));
		memcpy(Undersampled_data.memptr(), GetDataArray<complex<float>>(data), input_data.GetWidth() * input_data.GetHeight() * sizeof(complex<float>));

		fmat mask = trans(Mask);
		cx_fmat undersampled_data = trans(Undersampled_data);

		ParametterSet myparameter = { 200, pow(10,-30), 8, 1, 0.002, 0.005, 0.01, 0.8, 2, mask, undersampled_data};

		auto recon_data = Reconstruct(undersampled_data, myparameter);
		complex<float> * recon = nullptr;
		try
		{
			recon = new complex<float>[input_data.GetWidth() * input_data.GetHeight()];
		}
		catch (bad_alloc&)
		{
			return false;
		}
		cx_fmat Recon_data = trans(recon_data);
		memcpy(recon, Recon_data.memptr(), input_data.GetWidth() * input_data.GetHeight() * sizeof(complex<float>));

		auto output = YapShared(new ComplexFloatData(recon, data->GetDimensions(), nullptr, true));

		Feed(L"Output", output.get());
	}
		return true;
}

arma::cx_fmat Yap::CompressedSensing::Reconstruct(arma::cx_fmat k_space, ParametterSet& myparameter)
{
	_iteration_count = 0;
	float eps = 2.22044604925031 * pow(10, -16);

	cx_fmat g0 = ComputeGradient(k_space, myparameter.wavelet_weight, 
		myparameter.tv_weight, myparameter.typenorm, myparameter.mask, myparameter.undersampling_k_space);
	cx_fmat differential_recon_data = -g0;

	while (1)
	{
		auto recon_image = IFft2DTransform(k_space);
		auto differential_recon_image = IFft2DTransform(differential_recon_data);

		//prepare for calculate fidelity energy；
		auto recon_k_data = k_space % myparameter.mask;
		auto differential_recon_k_data = differential_recon_data % myparameter.mask;

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
		auto temp_g0 = conj(g0) * differential_recon_data;
		auto sum_temp_g0 = accu(temp_g0);
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
			return cx_fmat();
		}
		if (line_search_times > 2)
		{
			myparameter.initial_line_search_step = myparameter.initial_line_search_step * myparameter.line_search_beta;
		}
		if (line_search_times < 1)
		{
			myparameter.initial_line_search_step = myparameter.initial_line_search_step / myparameter.line_search_beta;
		}

		k_space = k_space + differential_recon_data * step;

		cx_fmat g1 = ComputeGradient(k_space, myparameter.wavelet_weight, myparameter.tv_weight, 
			myparameter.typenorm, myparameter.mask, myparameter.undersampling_k_space);

		auto temp_g1 = abs(g1) * abs(g1);
		auto sum_energy_g1 = accu(temp_g1);
		//auto module_g0 = g0.module();								//
		auto power_module_g0 = abs(g0) * abs(g0);		//
		auto sum_energy_g0 = accu(power_module_g0);		//
		float ellipse_factor = sum_energy_g1 / (sum_energy_g0 + eps);

		g0 = g1;
		differential_recon_data = differential_recon_data * ellipse_factor - g1;
		++_iteration_count;
		auto temp_differential_reconstruct_data = abs(differential_recon_data);
		auto differential_reconstruct_data_norm = accu(temp_differential_reconstruct_data);
		if ((_iteration_count > myparameter.max_conjugate_gradient_iteration_times) ||
			(differential_reconstruct_data_norm < myparameter.gradient_tollerance))
		{
			break;
		}
	}
	return k_space;
}

arma::cx_fmat Yap::CompressedSensing::ComputeGradient(cx_fmat in_data, float wavelet_weight, float tv_weight, float p_norm, fmat mask, cx_fmat subsampled_kdata)
{
	cx_fmat gradient = GetFidelityTerm(in_data, mask, subsampled_kdata);
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

arma::cx_fmat Yap::CompressedSensing::GetFidelityTerm(cx_fmat in_data, fmat mask, cx_fmat subsampled_kdata)
{
	return (in_data % mask - subsampled_kdata) * 2.0;
}

arma::cx_fmat Yap::CompressedSensing::GetWaveletTerm(cx_fmat in_data, float p_norm)
{
	cx_fmat Wavelet_data = Fw2DTransform(in_data);
	cx_fmat wavelet_data = trans(Wavelet_data);
	auto wavelet_cursor = wavelet_data.begin();
	auto wavelet_cursor_end = wavelet_data.end();

	float epsilon = pow(10, -15);
	for (; wavelet_cursor < wavelet_cursor_end; ++wavelet_cursor)
	{
		double module_square = norm(*wavelet_cursor);
		*wavelet_cursor *= pow(module_square + epsilon, p_norm / 2.0 - 1.0) * p_norm;
	}
	return Fft2DTransform(IFw2DTransform(trans(wavelet_data)));
}

arma::cx_fmat Yap::CompressedSensing::GetTVTerm(cx_fmat in_data, float p_norm)
{
	auto dx = TV2DTransform(in_data);
	float epsilon = pow(10, -15);

	for (unsigned int i = 0; i < 2; ++i)
	{
		cx_fmat Tv = dx.slice(i);
		cx_fmat tv = trans(Tv);
		auto cursor = tv.begin();
		auto end_cursor = tv.end();
		for (; cursor < end_cursor; ++cursor)
		{
			float module_square = norm(*cursor);
			*cursor *= pow(module_square + epsilon, p_norm / 2.0 - 1.0) * p_norm;
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

void Yap::CompressedSensing::GenerateFilter(wchar_t* filter_type_name, unsigned int coarse_level)
{
	_filter.clear();
	assert(filter_type_name != NULL);
	SetFilterParams(coarse_level);
	
	ifstream filter_file(filter_type_name);
	while (filter_file)
	{
		float fdata;
		filter_file >> fdata;
		_filter.push_back(fdata);
	}

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

arma::fmat Yap::CompressedSensing::DownSampling(fmat Output, std::vector<float>& filter, unsigned int nc)
{
	fmat output = trans(Output);
	auto width = output.n_cols;
	auto top = output.memptr() + nc / 2;//变换后所用的位置+ 1
	auto bot = output.memptr();
	for (unsigned int ix = 0; ix < nc; ++ix)//行变换
	{
		std::vector<float> row(nc);//把一行所有的元素提取出来，低通放左边，高通放右边
		memcpy(row.data(), bot + ix * width, nc * sizeof(float));
		//左部分
		memcpy(bot + ix * width, DownSamplingLowPass(row, filter).data(), (nc / 2) * sizeof(float));
		//右部分
		memcpy(top + ix * width, DownSamplingHighPass(row, filter).data(), (nc / 2) * sizeof(float));
	}
	return trans(output);
}


arma::fmat Yap::CompressedSensing::IFWT2D(arma::fmat Input_data, std::vector<float>& filter, unsigned int scale, ILongTaskListener * listener /*= nullptr*/)
{
	fmat input = trans(Input_data);
	auto width = input.n_cols;
	auto height = input.n_rows;
	assert(width == height);

	fmat output(height, width);
	memcpy(output.memptr(), input.memptr(), width * height * sizeof(float));
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
		output = UpSampling(output, filter, nc);
		output = UpSampling(trans(output), filter, nc);
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
	return trans(output);
}

arma::fmat Yap::CompressedSensing::UpSampling(arma::fmat Output_data, std::vector<float>& filter, unsigned int nc)
{
	fmat output = trans(Output_data);
	auto width = output.n_cols;
	auto top = output.memptr() + nc / 2;					// 变换后所用的位置 + 1
	auto bot = output.memptr();
	for (unsigned int ix = 0; ix < nc; ++ix)				 // 行变换
	{
		//左部分
		vector<float> bot_row(nc / 2);
		memcpy(bot_row.data(), bot + ix * width, (nc / 2) * sizeof(float));
		vector<float> front(nc);
		memcpy(front.data(), UpSamplingLowPass(bot_row, filter).data(), nc * sizeof(float));
		//右部分
		vector<float> top_row(nc / 2);
		memcpy(top_row.data(), top + ix * width, (nc / 2) * sizeof(float));
		vector<float> back(nc);
		memcpy(back.data(), UpSamplingHighPass(top_row, filter).data(), nc * sizeof(float));
		vector<float> row(nc);
		for (unsigned int i = 0; i < nc; ++i)
		{
			row[i] = front[i] + back[i];
		}
		memcpy(bot + ix * width, row.data(), nc * sizeof(float));
	}
	return trans(output);
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

arma::cx_fcube Yap::CompressedSensing::TV2DTransform(arma::cx_fmat In_data)
{
	cx_fmat in_data = trans(In_data);
	unsigned int width = in_data.n_cols;
	unsigned int height = in_data.n_rows;

	cx_fcube tv_result(width, height, 2);
	auto& Vertical_tv = tv_result.slice(0);
	cx_fmat vertical_tv = trans(Vertical_tv);

	complex<float> * cursor = vertical_tv.begin();
	complex<float> * cursor_end = vertical_tv.end() - 1;
	complex<float> * source_cursor;
	for (source_cursor = in_data.memptr(); cursor < cursor_end; ++cursor, ++source_cursor)
	{
		*cursor = *(source_cursor + width) - (*source_cursor);
	}
	tv_result.slice(0) = trans(vertical_tv);
	auto& Horizontal_tv = tv_result.slice(1);
	cx_fmat horizontal_tv = trans(Horizontal_tv);
	cursor = horizontal_tv.memptr();
	source_cursor = in_data.memptr();

	for (unsigned int row = 0; row < height; ++row)
	{
		for (unsigned int column = 0; column < width - 1; ++column)
		{
			*cursor = *(source_cursor + 1) - *source_cursor;
			++cursor;
			++source_cursor;
		}
		++cursor;
		++source_cursor;
	}
	tv_result.slice(1) = trans(horizontal_tv);
	return tv_result;
}

arma::cx_fmat Yap::CompressedSensing::ITV2DTransform(arma::cx_fcube in_data)
{
	unsigned int width = in_data.slice(0).n_cols;
	unsigned int height = in_data.slice(0).n_rows;

	cx_fmat itv_result(width, height);
	cx_fmat temp_x(width, height);
	cx_fmat temp_y(width, height);

	auto& Vertical_tv = in_data.slice(0);
	cx_fmat vertical_tv = trans(Vertical_tv);
	for (unsigned int i = 0; i < width * (height - 1); ++i)
	{
		if (i < width)
			temp_x[i] = vertical_tv[i] * (-1.0f);
		else
			temp_x[i] = vertical_tv[i - width] - vertical_tv[i];
	}
	for (unsigned int i = width * (height - 1); i < width * height; ++i)
	{
		temp_x[i] = vertical_tv[i - width];
	}

	auto& Horizontal_tv = in_data.slice(1);
	cx_fmat horizontal_tv = trans(Horizontal_tv);
	for (unsigned int i = 0; i < width * height; ++i)
	{
		if (i % width != 0)
		{
			temp_y[i] = horizontal_tv[i - 1] - horizontal_tv[i];
		}
		else
		{
			temp_y[i] = horizontal_tv[i] * (-1.0f);
			if (i != 0)
				temp_y[i - 1] = horizontal_tv[i - 2];
		}
		temp_y[width * height - 1] = horizontal_tv[width * height - 2];
	}
	itv_result = temp_x + temp_y;

	return trans(itv_result);
}

arma::cx_fmat Yap::CompressedSensing::Fft2DTransform(arma::cx_fmat In_data)
{
	cx_fmat image_slice;
	cx_fmat in_data = trans(In_data);
	image_slice.resize(in_data.n_cols, in_data.n_rows);
	FftShift(in_data);  // 由于小波变换问题，必须把k-space先fftshift
	
		//从图像到k空间是傅里叶变换
		fftwf_execute_dft(_p_FFT, (fftwf_complex*)in_data.memptr(), (fftwf_complex*)image_slice.memptr());
	if (_fft_shift)
	{
		FftShift(image_slice);
	}

	image_slice /= sqrt(in_data.n_elem);

	return trans(image_slice);
}

void Yap::CompressedSensing::SetFFTPlan(const fftwf_plan& plan)
{
	_p_FFT = plan;
}

void Yap::CompressedSensing::SetIFFTPlan(const fftwf_plan& plan)
{
	_p_IFFT = plan;
}

void Yap::CompressedSensing::FftShift(arma::cx_fmat data)
{
	auto width = data.n_cols;
	auto height = data.n_rows;
	SwapBlock(data.memptr(), data.memptr() + height / 2 * width + width / 2, width / 2, height / 2, width);
	SwapBlock(data.memptr() + width / 2, data.memptr() + height / 2 * width, width / 2, height / 2, width);
}

void Yap::CompressedSensing::SwapBlock(std::complex<float> * block1, std::complex<float> * block2, 
	unsigned int width, unsigned int height, unsigned int line_stride)
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

arma::fmat Yap::CompressedSensing::FWT2D(arma::fmat Input_data, std::vector<float>& filter, unsigned int scale, ILongTaskListener * listener /*= nullptr*/)
{
	fmat input = trans(Input_data);
	auto width = input.n_cols;
	auto height = input.n_rows;
	assert(width == height);
	fmat output(width, height);
	memcpy(output.memptr(), input.memptr(), width * height * sizeof(float));

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
		output = DownSampling(trans(output), filter, nc);// 行方向下采样
		output = DownSampling(trans(output), filter, nc);//列方向下采样
		fmat output = trans(output);
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
	return trans(output);
}

arma::cx_fmat Yap::CompressedSensing::Fw2DTransform(arma::cx_fmat input, ILongTaskListener * listener /*= nullptr*/)
{
	int t = sizeof(int);	//
	cx_fmat in_data(input);
	auto width = in_data.n_cols;
	auto height = in_data.n_rows;
	fmat in_real_part(in_data.n_cols, in_data.n_rows);
	fmat in_imaginary_part(in_data.n_cols, in_data.n_rows);
	cx_fmat wave_data;
	wave_data.resize(in_data.n_cols, in_data.n_rows);
	//提取实部、虚部
	for (unsigned int i = 0; i < in_data.n_elem; ++i)
	{
		in_real_part[i] = in_data[i].real();
		in_imaginary_part[i] = in_data[i].imag();
	}
	//CGenerateFilter filter(_T("db"), 3, 4); //第二个参数决定基的类型，第三个参数决定小波级数
	GenerateFilter(L"daub2", 4);
	//小波变换
	auto output_real_part = FWT2D(in_real_part, _filter, _coarse_level, listener);
	auto output_imaginary_part = FWT2D(in_imaginary_part, _filter, _coarse_level, listener);
	//组合复数
	for (unsigned int i = 0; i < in_data.n_elem; ++i)
	{
		wave_data[i] = std::complex<float>(output_real_part[i], output_imaginary_part[i]);
	}

	return wave_data;
}

arma::cx_fmat Yap::CompressedSensing::IFw2DTransform(cx_fmat input, ILongTaskListener * listener /*= nullptr*/)
{
	cx_fmat in_data(input);
	auto width = in_data.n_cols;
	auto height = in_data.n_rows;
	fmat in_real_part(in_data.n_cols, in_data.n_rows);
	fmat in_imaginary_part(in_data.n_cols, in_data.n_rows);
	cx_fmat iwave_data;
	iwave_data.resize(in_data.n_cols, in_data.n_rows);

	//提取实部、虚部
	for (unsigned int i = 0; i < in_data.n_elem; ++i)
	{
		in_real_part[i] = in_data[i].real();
		in_imaginary_part[i] = in_data[i].imag();
	}
	GenerateFilter(L"daub2", 4);
	//反小波变换
	auto output_real_part = IFWT2D(in_real_part, _filter, _coarse_level, listener);
	auto output_imaginary_part = IFWT2D(in_imaginary_part, _filter, _coarse_level, listener);
	//组合复数
	for (unsigned int i = 0; i < in_data.n_elem; ++i)
	{
		iwave_data[i] = std::complex<float>(output_real_part[i], output_imaginary_part[i]);
	}
	return iwave_data;
}

float Yap::CompressedSensing::CalculateEnergy(arma::cx_fmat recon_k_data, 
	arma::cx_fmat differential_recon_k_data, arma::cx_fmat recon_wavelet_data, 
	arma::cx_fmat differential_recon_wavelet_data, arma::cx_fcube recon_tv_data, 
	arma::cx_fcube differential_recon_tv_data, const ParametterSet& myparameter, float step_length)
{
	float total_energy = 0.0;
	float fidelity_energy = 0.0;
	auto temp_fidelity_energy = recon_k_data + differential_recon_k_data * step_length - myparameter.undersampling_k_space;

	auto power_module_temp_fidelity_energy = abs(temp_fidelity_energy) * abs(temp_fidelity_energy);
	fidelity_energy = accu(power_module_temp_fidelity_energy);
	float epsilon = pow(10, -15);

	float wavelet_energy = 0.0;
	if (myparameter.wavelet_weight)
	{
		auto temp_wavelet_energy = recon_wavelet_data + differential_recon_wavelet_data * step_length;
		//auto module_wavelet_energy = temp_wavelet_energy.module();
		auto power_module_temp_wavelet_energy = abs(temp_wavelet_energy) * abs(temp_wavelet_energy) + epsilon;  //模的平方
		//auto temp = DataType::power(power_module_temp_wavelet_energy, p_norm/2);
		//数学上等同于上式：p_norm = 1；
		auto temp = sqrt(power_module_temp_wavelet_energy);
		wavelet_energy = accu(temp) * myparameter.wavelet_weight;
	}

	float tv_energy = 0.0;
	if (myparameter.tv_weight)
	{
		for (unsigned int i = 0; i < 2; ++i)
		{
			auto temp_tv_energy = recon_tv_data.slice(i) + differential_recon_tv_data.slice(i) * step_length;
			//auto module_tv_energy = temp_tv_energy.module();
			auto power_module_temp_tv_energy = abs(temp_tv_energy) * abs(temp_tv_energy) + epsilon;			//模的平方
		    //auto temp = DataType::power(power_module_temp_tv_energy, p_norm/2);
			//数学意义上等同于上式：p_norm = 1
			auto temp = sqrt(power_module_temp_tv_energy);
			tv_energy += accu(temp) * myparameter.tv_weight;
		}
	}

	total_energy = fidelity_energy + wavelet_energy + tv_energy;
	return total_energy;
}

arma::cx_fmat Yap::CompressedSensing::IFft2DTransform(arma::cx_fmat In_data)
{
	cx_fmat image_slice;
	cx_fmat in_data = trans(In_data);
	image_slice.resize(in_data.n_cols, in_data.n_rows);
	FftShift(in_data);  // 由于小波变换问题，必须把k-space先fftshift

		//从k空间到图像是反傅里叶变换
		fftwf_execute_dft(_p_IFFT, (fftwf_complex*)in_data.memptr(), (fftwf_complex*)image_slice.memptr());
	if (_fft_shift)
	{
		FftShift(image_slice);
	}

	image_slice /= sqrt(in_data.n_cols * in_data.n_rows);

	return trans(image_slice);
}
