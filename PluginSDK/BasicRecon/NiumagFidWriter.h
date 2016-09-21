#pragma once
#ifndef NiumagFidWriter_h__
#define NiumagFidWriter_h__

#include "Interface/Implement/processorImpl.h"

namespace Yap
{
	class NiumagFidWriter :
		public ProcessorImpl
	{
	public:
		NiumagFidWriter(void);
		NiumagFidWriter(const NiumagFidWriter& rhs);

	private:
		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;
	};

}

#endif // NiumagFidWriter_h__

