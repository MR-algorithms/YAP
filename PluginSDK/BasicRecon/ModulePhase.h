#pragma once

#ifndef ModulePhase_h__20160814
#define ModulePhase_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class ModulePhase :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(ModulePhase)
	public:
		ModulePhase(void);
		ModulePhase(const ModulePhase& rhs);
		
		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		~ModulePhase();
	};
}
#endif // ModulePhase_h__
