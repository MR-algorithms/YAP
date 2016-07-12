#pragma once
#include "ProcessorImp.h"
#include <complex>

namespace Yap
{
	class CRemoveDC :
		public CProcessorImp
	{
	public:
		CRemoveDC();
		~CRemoveDC();

		virtual bool Input(const wchar_t * port, IData * data) override;

	};
}

