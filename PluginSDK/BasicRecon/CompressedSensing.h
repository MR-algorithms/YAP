#pragma once
#include "Interface/Implement/ProcessorImpl.h"
#include "fftw3.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <complex>
#include <boost/numeric/ublas/fwd.hpp>


namespace Yap
{
	struct ParametterSet
	{
		unsigned int max_line_search_times;
		float gradient_tollerance;
		unsigned int max_conjugate_gradient_iteration_times;
		unsigned int typenorm;
		float tv_weight;
		float wavelet_weight;
		float line_search_alpha;
		float line_search_beta;
		float initial_line_search_step;
		boost::numeric::ublas::matrix<float> mask;
		boost::numeric::ublas::matrix<std::complex<float>> undersampling_k_space;
	};

	struct  ILongTaskListener
	{
		virtual void OnBegin() = 0;
		virtual bool OnProgress(unsigned int percent) = 0;
		virtual void OnEnd() = 0;
	};

	class CompressedSensing :
		public ProcessorImpl
	{
	public:
		CompressedSensing();
		virtual ~CompressedSensing(void);

		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;

	protected:
		SmartPtr<IData> _mask;

		boost::numeric::ublas::matrix<std::complex<float>> Reconstruct(boost::numeric::ublas::matrix<std::complex<float>>& subsampled_data, ParametterSet& myparameter);

		boost::numeric::ublas::matrix<std::complex<float>> ComputeGradient(boost::numeric::ublas::matrix < std::complex<float >> &in_data, float wavelet_weight, float tv_weight,
			float p_norm, boost::numeric::ublas::matrix<float> mask, boost::numeric::ublas::matrix<std::complex<float>> subsampled_kdata);

		boost::numeric::ublas::matrix<std::complex<float>> GetFidelityTerm(boost::numeric::ublas::matrix<std::complex<float>>& in_data, 
			boost::numeric::ublas::matrix<float>& mask, boost::numeric::ublas::matrix<std::complex<float>> subsampled_kdata);
		boost::numeric::ublas::matrix<std::complex<float>> GetWaveletTerm(boost::numeric::ublas::matrix<std::complex<float>>& in_data, float p_norm);
		boost::numeric::ublas::matrix<std::complex<float>> GetTVTerm(boost::numeric::ublas::matrix<std::complex<float>>& in_data, float p_norm);
		float CalculateEnergy(boost::numeric::ublas::matrix<std::complex<float>>& recon_k_data, boost::numeric::ublas::matrix<std::complex<float>>& differential_recon_kdata,
			boost::numeric::ublas::matrix<std::complex<float>>& recon_wavelet_data, boost::numeric::ublas::matrix<std::complex<float>>& differential_recon_wavelet_data, 
			std::vector<boost::numeric::ublas::matrix<std::complex<float>>>& recon_tv_data, std::vector<boost::numeric::ublas::matrix<std::complex<float>>>& differential_recon_tv_data, 
			const ParametterSet& myparameter, float step_length);


		//Fwt
		unsigned int QuadLength(unsigned int length);
		std::vector<float> Iconv(std::vector<float>& filter, std::vector<float>& row);
		boost::numeric::ublas::matrix<std::complex<float>> Fw2DTransform(boost::numeric::ublas::matrix<std::complex<float>>& input, ILongTaskListener * listener = nullptr);
		boost::numeric::ublas::matrix<float> FWT2D(boost::numeric::ublas::matrix<float>& input, std::vector<float>& filter, unsigned int scale, ILongTaskListener * listener = nullptr);
		std::vector<float> Filter(std::vector<float>& filter, unsigned int a, std::vector<float>& in_put);
		std::vector<float> MirrorFilt(std::vector<float>& filter);
		std::vector<float> Aconv(std::vector<float>& filter, std::vector<float>& row);
		void SetFilterParams(unsigned int filter_size);
		void GenerateFilter(wchar_t * filter_type_name, unsigned int coarse_level);
		void SetFilter(float * filter, unsigned int size);
		std::vector<float> DownSamplingLowPass(std::vector<float>& row, std::vector<float>& filter);
		std::vector<float> DownSamplingHighPass(std::vector<float>& row, std::vector<float>& filter);
		std::vector<float> LeftShift(std::vector<float>& row);
		boost::numeric::ublas::matrix<float> DownSampling(boost::numeric::ublas::matrix<float>& output, std::vector<float>& filter, unsigned int nc);

		//Ifwt
		boost::numeric::ublas::matrix<std::complex<float>> IFw2DTransform(boost::numeric::ublas::matrix<std::complex<float>>& input, ILongTaskListener * listener = nullptr);
		boost::numeric::ublas::matrix<float> IFWT2D(boost::numeric::ublas::matrix<float>& input, std::vector<float>& filter, unsigned int scale, ILongTaskListener * listener = nullptr);
		boost::numeric::ublas::matrix<float> UpSampling(boost::numeric::ublas::matrix<float>& input, std::vector<float>& filter, unsigned int nc);
		std::vector<float> UpSamplingLowPass(std::vector<float>& row, std::vector<float>& filter);
		std::vector<float> UpSampleInterpolateZero(std::vector<float>& row);
		std::vector<float> UpSamplingHighPass(std::vector<float>& row, std::vector<float>& filter);
		std::vector<float> RightShift(std::vector<float>& row);

		//TV
		std::vector<boost::numeric::ublas::matrix<std::complex<float>>> TV2DTransform(boost::numeric::ublas::matrix<std::complex<float>>& in_data);
		//ITV2D
		boost::numeric::ublas::matrix<std::complex<float>> ITV2DTransform(std::vector<boost::numeric::ublas::matrix<std::complex<float>>>& input);

		//Fft
		boost::numeric::ublas::matrix<std::complex<float>> Fft2DTransform(boost::numeric::ublas::matrix<std::complex<float>> data);
		boost::numeric::ublas::matrix<std::complex<float>> IFft2DTransform(boost::numeric::ublas::matrix<std::complex<float>> data);
		void SetFFTPlan(const fftwf_plan& plan);
		void SetIFFTPlan(const fftwf_plan& plan);
		void FftShift(boost::numeric::ublas::matrix<std::complex<float>>& data);
		void SwapBlock(std::complex<float> * block1, std::complex<float> * block2,
			size_t width, size_t height, unsigned int line_stride);

		boost::numeric::ublas::matrix<float> Transpose(boost::numeric::ublas::matrix<float>& in_data);
		boost::numeric::ublas::matrix<float> square_module(boost::numeric::ublas::matrix<std::complex<float>> input);
		float sum(boost::numeric::ublas::matrix<float> input);
		std::complex<float> sum(boost::numeric::ublas::matrix<std::complex<float>> input);
		boost::numeric::ublas::matrix<float> fill(float value, boost::numeric::ublas::matrix<float>& input);
		boost::numeric::ublas::matrix<float> sqrt_root(boost::numeric::ublas::matrix<float> input);
		boost::numeric::ublas::matrix<std::complex<float>> dot_multiply(boost::numeric::ublas::matrix<float> mask, boost::numeric::ublas::matrix<std::complex<float>> input);
		boost::numeric::ublas::matrix<std::complex<float>> conj_multiply(boost::numeric::ublas::matrix<std::complex<float>>& input_1, boost::numeric::ublas::matrix<std::complex<float>>& input_2);
		boost::numeric::ublas::matrix<float> module(boost::numeric::ublas::matrix<std::complex<float>> input);
		boost::numeric::ublas::matrix<float> GetMask(float * mask, size_t width, size_t height);
		boost::numeric::ublas::matrix<std::complex<float>> GetSubsampledData(std::complex<float>* data, size_t width, size_t height);
		void GetReconData(std::complex<float> * recon, boost::numeric::ublas::matrix<std::complex<float>>& data);
		boost::numeric::ublas::matrix<std::complex<float>> Matrix_plus(boost::numeric::ublas::matrix<std::complex<float>>& input_1, 
			boost::numeric::ublas::matrix<std::complex<float>>& input_2);
		boost::numeric::ublas::matrix<float> Matrix_plus(boost::numeric::ublas::matrix<float>& input_1,
			boost::numeric::ublas::matrix<float>& input_2);
		boost::numeric::ublas::matrix<std::complex<float>> Matrix_minus(boost::numeric::ublas::matrix<std::complex<float>> input_1,
			boost::numeric::ublas::matrix<std::complex<float>> input_2);
		boost::numeric::ublas::matrix<std::complex<float>> Matrix_scale_multiply(boost::numeric::ublas::matrix<std::complex<float>>& input, float value);
		boost::numeric::ublas::matrix<std::complex<float>> Matrix_scale_div(boost::numeric::ublas::matrix<std::complex<float>>& input, float value);
		boost::numeric::ublas::matrix<std::complex<float>> Minus(boost::numeric::ublas::matrix<std::complex<float>>& input);
		boost::numeric::ublas::matrix<float> GetRealPart(boost::numeric::ublas::matrix<std::complex<float>> input);
		boost::numeric::ublas::matrix<float> GetImaginaryPart(boost::numeric::ublas::matrix<std::complex<float>> input);

	private:
		std::vector<float> _filter;
		unsigned int _filter_type_size;
		unsigned int _coarse_level;
		unsigned int _iteration_count;

		fftwf_plan _p_FFT;
		fftwf_plan _p_IFFT;
	};
}

