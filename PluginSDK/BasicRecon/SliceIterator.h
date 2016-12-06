#pragma once
#ifndef SliceIterator_h__20160814
#define SliceIterator_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class SliceIterator :
		public ProcessorImpl
	{
	public:
		SliceIterator(void);
		SliceIterator(const SliceIterator& rhs);

		virtual IProcessor* Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;

	protected:
		~SliceIterator(void);
	};
}
#endif // SliceIterator_h__
