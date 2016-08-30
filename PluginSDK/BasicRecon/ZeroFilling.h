#pragma once

#ifndef ZeroFilling_h__20160814
#define ZeroFilling_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class CZeroFilling :
		public ProcessorImpl
	{
	public:
		CZeroFilling();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:

		virtual ~CZeroFilling();

		virtual IProcessor * Clone() override;
	};
}
#endif // ZeroFilling_h__
