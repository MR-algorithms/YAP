#ifndef Fft1D_h__20160813
#define Fft1D_h__20160813

#pragma once
#include "Implement/ProcessorImpl.h"
#include "Client/DataHelper.h"
#include "fftw3.h"
#include <complex>
#include <vector>

namespace Yap
{
	class Fft1D :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(Fft1D)
	public:
		Fft1D();
		Fft1D(const Fft1D& rhs);

	protected:
		~Fft1D();

		virtual bool Input(const wchar_t * port, IData * data) override;

		template <typename T>
		void FFTShift(std::complex<T>* data, size_t size);

		template <typename T>
		void SwapBlock(std::complex<T> * block1, std::complex<T> * block2,
			size_t width);

		unsigned int _plan_data_size;
		bool _plan_inverse;
		bool _plan_in_place;

		fftwf_plan _fft_plan_float;
		fftw_plan _fft_plan_double;

		template<typename T> bool DoFft(IData * data, size_t size);

		template<typename T>
		bool Fft(std::complex<T> * data, std::complex<T> * result,
			size_t size, bool inverse = false);

		template<typename T>
		void Plan(size_t size, bool inverse, bool in_place);
	};
}

#endif // Fft1D_h__
