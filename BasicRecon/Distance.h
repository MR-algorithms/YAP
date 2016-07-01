#pragma once
#include "ProcessorImp.h"

namespace Yap
{
	class CDistance :
		public CProcessorImp
	{
	public:
		CDistance();
		virtual ~CDistance();

		virtual bool Input(const wchar_t * port, IData * data) override;

	};

}

