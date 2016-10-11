#pragma once
#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class SumOfSquare :
		public ProcessorImpl
	{
	public:
		SumOfSquare();
		virtual ~SumOfSquare();

		virtual bool OnInit() override;

		virtual IProcessor * Clone() override;

		virtual bool Input(const wchar_t * name, IData * data) override;

	private:
		std::vector<float> SumofSquare(std::complex<float> * input_data, unsigned int width, unsigned int height, unsigned int coil_count);
	};
}

