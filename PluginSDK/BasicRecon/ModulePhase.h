#pragma once

#ifndef ModulePhase_h__20160814
#define ModulePhase_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class CModulePhase :
		public ProcessorImpl
	{
	public:
		CModulePhase();
		virtual ~CModulePhase();

		virtual bool Input(const wchar_t * port, IData * data) override;
		virtual IProcessor * Clone() override;

	};
}
#endif // ModulePhase_h__
