#pragma once

#ifndef RemoveDC_h__20160814
#define RemoveDC_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class CRemoveDC :
		public ProcessorImpl
	{
	public:
		CRemoveDC();
		~CRemoveDC();

		virtual bool Input(const wchar_t * port, IData * data) override;

		virtual IProcessor * Clone() override;

	};
}

#endif // RemoveDC_h__
