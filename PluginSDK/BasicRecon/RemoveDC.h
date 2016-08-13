#pragma once

#ifndef RemoveDC_h__20160814
#define RemoveDC_h__20160814

#include "Tools/ProcessorImp.h"

namespace Yap
{
	class CRemoveDC :
		public CProcessorImp
	{
	public:
		CRemoveDC();
		~CRemoveDC();

		virtual bool Input(const wchar_t * port, IData * data) override;

		virtual IProcessor * Clone() override;

	};
}

#endif // RemoveDC_h__
