#pragma once

#ifndef SliceSelector_h__20160814
#define SliceSelector_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class SliceSelector :
		public ProcessorImpl
	{
	public:
		SliceSelector(void);
		SliceSelector(const SliceSelector& rhs);
		virtual ~SliceSelector();

		virtual IProcessor * Clone() override;

		virtual bool Input(const wchar_t * name, IData * data) override;

		// Inherited via ProcessorImpl
		virtual bool OnInit() override;

	};
}
#endif // SliceSelector_h__


