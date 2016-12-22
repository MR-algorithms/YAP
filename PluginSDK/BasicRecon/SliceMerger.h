#pragma once
#ifndef SliceIterator_h__20161221
#define SliceIterator_h__20161221

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class SliceMerger :
		public ProcessorImpl
	{
	public:
		SliceMerger(void);
		SliceMerger(const SliceMerger& rhs);

		virtual IProcessor* Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;

	protected:
		~SliceMerger(void);

		SmartPtr<IData> _data;
	};
}
#endif // SliceIterator_h__