#pragma once

#ifndef ZeroFilling_h__20160814
#define ZeroFilling_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class ZeroFilling :
		public ProcessorImpl
	{
	public:
		ZeroFilling();
		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		~ZeroFilling();

		virtual IProcessor * Clone() override;
	};
}
#endif // ZeroFilling_h__
