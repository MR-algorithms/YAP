#pragma once
#ifndef SliceIterator_h__20160814
#define SliceIterator_h__20160814

#include "Tools/ProcessorImp.h"

namespace Yap
{
	class CSliceIterator :
		public CProcessorImp
	{
	public:
		CSliceIterator(void);
		CSliceIterator(const CSliceIterator& rhs);
		~CSliceIterator(void);
		virtual IProcessor* Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;
	};

}
#endif // SliceIterator_h__
