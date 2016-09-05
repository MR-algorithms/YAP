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
		ModulePhase();
		virtual ~ModulePhase();

		virtual bool Input(const wchar_t * port, IData * data) override;
		virtual IProcessor * Clone() override;

		// Inherited via ProcessorImpl
		virtual bool OnInit() override;
	};
}
#endif // ModulePhase_h__
