#pragma once
#include "ProcessorImp.h"
#include "DataHelper.h"
#include "fftw3.h"
#include <complex>
#include <vector>

namespace Yap
{
	class CFft1D :
		public CProcessorImp
	{
	public:
		CFft1D();
		virtual ~CFft1D();

		virtual bool Input(const wchar_t * port, IData * data) override;

		void FFTShift(std::complex<double>* data, size_t size);

	protected:

		void SwapBlock(std::complex<double> * block1, std::complex<double> * block2,
			size_t width);

		unsigned int _plan_data_size;
		bool _plan_inverse;
		bool _plan_in_place;
		fftw_plan _fft_plan;

		bool Fft1D(std::complex<double> * data, std::complex<double> * result,
			size_t size, bool inverse = false);
		void Plan(size_t size, bool inverse, bool in_place);

	};
}



