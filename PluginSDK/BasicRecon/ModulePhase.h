#pragma once

#ifndef ModulePhase_h__20160814
#define ModulePhase_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class ModulePhase :
		public ProcessorImpl
	{
	public:
		ModulePhase(void);
		ModulePhase(const ModulePhase& rhs);

		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * port, IData * data) override;
	protected:
		~ModulePhase();

	};
}
#endif // ModulePhase_h__
