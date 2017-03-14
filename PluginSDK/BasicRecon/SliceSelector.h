#pragma once

#ifndef SliceSelector_h__20160814
#define SliceSelector_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class SliceSelector :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(SliceSelector)
	public:
		SliceSelector(void);
		SliceSelector(const SliceSelector& rhs);
		
		virtual bool Input(const wchar_t * name, IData * data) override;

	protected:
		~SliceSelector();
	};
}
#endif // SliceSelector_h__


