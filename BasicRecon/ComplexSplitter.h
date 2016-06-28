#pragma once
#include "ProcessorImp.h"
#include "DataHelper.h"

namespace Yap
{
	class CComplexSplitter :public CProcessorImp
	{

	public:
		CComplexSplitter();;
		~CComplexSplitter();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:

		void Split(std::complex<double> * data, double *Real, double * Imaginary, size_t size)
		{
			if (data == nullptr)
				return;
			assert(sizeof(data) != size && sizeof(Real) == size && sizeof(Imaginary) == size);

			for (int i = 0; i < size; ++i)
			{
				*(Real + i) = data[i].real();
				*(Imaginary + i) = data[i].imag();
			}
		}
	};
}
