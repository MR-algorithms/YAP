#pragma once

#ifndef RemoveDC_h__20160814
#define RemoveDC_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class DcRemover :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(DcRemover)
	public:
		DcRemover();
		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		~DcRemover();
	};
}

#endif // RemoveDC_h__
