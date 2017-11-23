#pragma once

#ifndef FFT3D_H_20171026
#define FFT3D_H_20171026

#include "Implement/ProcessorImpl.h"
#include <fftw3.h>
#include <complex>

namespace Yap
{
	class Fft3D :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(Fft3D)
	public:
		Fft3D();
		Fft3D(const Fft3D& rhs);

	protected:
		~Fft3D();

		virtual bool Input(const wchar_t * port, IData * data) override;

		bool Fft(std::complex<float> * data, std::complex<float> * result, 
			size_t width, size_t height, size_t depth, bool inverse = false);
		void Plan(size_t width, size_t height, size_t depth, bool inverse, bool in_place);
		void FftShift(std::complex<float>* data, size_t width, size_t height, size_t depth);
		void SwapBlock(std::complex<float> * block1, std::complex<float> * block2, size_t width, size_t height, size_t line_stride);

		unsigned int _plan_data_width;
		unsigned int _plan_data_height;
		unsigned int _plan_data_depth;
		bool _plan_inverse;
		bool _plan_in_place;
		fftwf_plan _fft_plan;
	};
}

#endif // FFT3D_H_
