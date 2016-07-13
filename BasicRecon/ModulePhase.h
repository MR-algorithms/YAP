#pragma once
#include "ProcessorImp.h"

namespace Yap
{
	class CModulePhase :
		public CProcessorImp
	{
	public:
		CModulePhase();
		virtual ~CModulePhase();

		virtual bool Input(const wchar_t * port, IData * data) override;

	};
}
