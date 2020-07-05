#pragma once

#ifndef SliceSelector_h__20160814
#define SliceSelector_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class SliceSelector :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(SliceSelector)
	public:
		SliceSelector(void);
		SliceSelector(const SliceSelector& rhs);

	protected:
		~SliceSelector();

		virtual bool Input(const wchar_t * name, IData * data) override;

		
	};
}
#endif // SliceSelector_h__


