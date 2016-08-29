#pragma once

#ifndef SliceSelector_h__20160814
#define SliceSelector_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class CSliceSelector :
		public ProcessorImpl
	{
	public:
		CSliceSelector(void);
		CSliceSelector(const CSliceSelector& rhs);
		virtual ~CSliceSelector();

		virtual IProcessor * Clone() override;

		virtual bool Input(const wchar_t * name, IData * data) override;

	};
}
#endif // SliceSelector_h__


