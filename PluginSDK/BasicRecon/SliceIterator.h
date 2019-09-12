#pragma once
#ifndef SliceIterator_h__20160814
#define SliceIterator_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class SliceIterator :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(SliceIterator)
	public:
		SliceIterator(void);
		SliceIterator(const SliceIterator& rhs);
		
	protected:
		~SliceIterator(void);

		virtual bool Input(const wchar_t * name, IData * data) override;
		bool AddSliceindexParam(IData *data, int index) const;
	};
}
#endif // SliceIterator_h__
