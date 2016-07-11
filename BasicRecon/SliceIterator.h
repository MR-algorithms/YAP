#pragma once
#include "ProcessorImp.h"

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
