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
		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * name, IData * data);
	};

}

#endif // NiumagFidWriter_h__

