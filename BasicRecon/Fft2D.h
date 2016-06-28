#pragma once
#include "ProcessorImp.h"
#include "DataHelper.h"
#include <fftw3.h>
#include <complex>
#include <vector>

namespace Yap 
{
	class CFft2D :
		public CProcessorImp
	{
	public:
		CFft2D();
		virtual ~CFft2D();

		virtual bool Input(const wchar_t * port, IData * data) override;

		void FftShift(std::complex<double>* data, size_t width, size_t height);

	protected:
		void SwapBlock(std::complex<double> * block1, std::complex<double> * block2, size_t width, size_t height, size_t line_stride);

		unsigned int _plan_data_width;
		unsigned int _plan_data_height;
		bool _plan_inverse;
		bool _plan_in_place;
		fftw_plan _fft_plan;

		bool Fft2D(std::complex<double> * data, std::complex<double> * result, size_t width, size_t height, bool inverse = false);
		void Plan(size_t width, size_t height, bool inverse, bool in_place);

	};
}
