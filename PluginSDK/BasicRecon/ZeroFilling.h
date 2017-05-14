#pragma once

#ifndef ZeroFilling_h__20160814
#define ZeroFilling_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class ZeroFilling :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(ZeroFilling)
	public:
		ZeroFilling();
		ZeroFilling(const ZeroFilling& rhs);

	protected:
		~ZeroFilling();

		virtual bool Input(const wchar_t * port, IData * data) override;
	};
}
#endif // ZeroFilling_h__
