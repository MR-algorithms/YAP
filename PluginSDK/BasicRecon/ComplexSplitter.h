#pragma once

#ifndef ComplexSplitter_h__20160813
#define ComplexSplitter_h__20160813

#include "Interface/Implement/ProcessorImpl.h"
#include "Interface/Client/DataHelper.h"

namespace Yap
{
	class ComplexSplitter :public ProcessorImpl
	{

	public:
		ComplexSplitter();;
		~ComplexSplitter();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:

		void Split(std::complex<double> * data, double * real, double * imaginary, size_t size);
		void ExtractReal(std::complex<double> * data, double * real, size_t size);
		void ExtractImaginary(std::complex<double> * data, double * imaginary, size_t size);

		virtual IProcessor * Clone() override;

		// Inherited via ProcessorImpl
		virtual bool OnInit() override;
	};
}
#endif // ComplexSplitter_h__
