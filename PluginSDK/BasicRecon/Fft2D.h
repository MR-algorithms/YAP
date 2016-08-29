#pragma once

#ifndef fft2d_h_20160813
#define fft2d_h_20160813

#include "Interface/Implement/ProcessorImpl.h"
#include "Interface/Client/DataHelper.h"
#include <fftw3.h>
#include <complex>

namespace Yap
{
	class CFft2D :
		public ProcessorImpl
	{
	public:
		CFft2D();
		virtual ~CFft2D();

		virtual bool Input(const wchar_t * port, IData * data) override;

		void FftShift(std::complex<float>* data, size_t width, size_t height);

	protected:
		void SwapBlock(std::complex<float> * block1, std::complex<float> * block2, size_t width, size_t height, size_t line_stride);

		unsigned int _plan_data_width;
		unsigned int _plan_data_height;
		bool _plan_inverse;
		bool _plan_in_place;
		fftwf_plan _fft_plan;

		bool Fft2D(std::complex<float> * data, std::complex<float> * result, size_t width, size_t height, bool inverse = false);
		void Plan(size_t width, size_t height, bool inverse, bool in_place);

		virtual IProcessor * Clone() override;

	};
}

#endif // !fft2d_h_20160813
