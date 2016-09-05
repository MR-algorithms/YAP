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

		virtual ~ZeroFilling();

		virtual IProcessor * Clone() override;

		// Inherited via ProcessorImpl
		virtual bool OnInit() override;
	};
}
#endif // ZeroFilling_h__
