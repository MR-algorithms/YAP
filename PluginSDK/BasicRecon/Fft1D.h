#ifndef Fft1D_h__20160813
#define Fft1D_h__20160813

#pragma once
#include "Interface/Implement/ProcessorImpl.h"
#include "Interface/Client/DataHelper.h"
#include "fftw3.h"
#include <complex>
#include <vector>

namespace Yap
{
	class Fft1D :
		public ProcessorImpl
	{
	public:
		Fft1D();
		virtual ~Fft1D();

		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		void FFTShift(std::complex<double>* data, size_t size);
		void SwapBlock(std::complex<double> * block1, std::complex<double> * block2,
			size_t width);

		unsigned int _plan_data_size;
		bool _plan_inverse;
		bool _plan_in_place;
		fftwf_plan _fft_plan;

		bool Fft(std::complex<double> * data, std::complex<double> * result,
			size_t size, bool inverse = false);
		void Plan(size_t size, bool inverse, bool in_place);
	};
}

#endif // Fft1D_h__
