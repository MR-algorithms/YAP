#pragma once

#ifndef fft2d_h_20160813
#define fft2d_h_20160813

#include "Implement/ProcessorImpl.h"
#include "Client/DataHelper.h"
#include <fftw3.h>
#include <complex>

namespace Yap
{
	class Fft2D :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(Fft2D)
	public:
		Fft2D();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		~Fft2D();

		void FftShift(std::complex<float>* data, size_t width, size_t height);
		void SwapBlock(std::complex<float> * block1, std::complex<float> * block2, size_t width, size_t height, size_t line_stride);

		unsigned int _plan_data_width;
		unsigned int _plan_data_height;
		bool _plan_inverse;
		bool _plan_in_place;
		fftwf_plan _fft_plan;

		bool Fft(std::complex<float> * data, std::complex<float> * result, size_t width, size_t height, bool inverse = false);
		void Plan(size_t width, size_t height, bool inverse, bool in_place);
	};
}

#endif // !fft2d_h_20160813
