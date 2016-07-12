#pragma once
#include "ProcessorImp.h"

namespace Yap
{
	class CRemoveDC :
		public CProcessorImp
	{
	public:
		CRemoveDC();
		~CRemoveDC();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		void RemoveDC(double * input_data, double * output_data, 
			size_t width, size_t height, bool inplace, size_t corner_size);
	};
}

