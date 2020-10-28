#pragma once

#ifndef ChannelSelector_h__20160814
#define ChannelSelector_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class ChannelSelector :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(ChannelSelector)
	public:
		ChannelSelector(void);
		ChannelSelector(const ChannelSelector& rhs);

	protected:
		~ChannelSelector();

		virtual bool Input(const wchar_t * name, IData * data) override;

		
	};
}
#endif // ChannelSelector_h__


