#pragma once

#ifndef ComplexSplitter_h__20160813
#define ComplexSplitter_h__20160813

#include "tools/ProcessorImp.h"
#include "Yap/DataHelper.h"

namespace Yap
{
	class CComplexSplitter :public CProcessorImp
	{

	public:
		CComplexSplitter();;
		~CComplexSplitter();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:

		void Split(std::complex<double> * data, double * real, double * imaginary, size_t size);
		void ExtractReal(std::complex<double> * data, double * real, size_t size);
		void ExtractImaginary(std::complex<double> * data, double * imaginary, size_t size);

		virtual IProcessor * Clone() override;

	};
}
#endif // ComplexSplitter_h__
