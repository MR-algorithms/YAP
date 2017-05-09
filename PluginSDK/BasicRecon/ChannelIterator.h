#pragma once
#include "Implement/ProcessorImpl.h"
namespace Yap
{
	class ChannelIterator :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(ChannelIterator)
	public:
		ChannelIterator();
		ChannelIterator(const ChannelIterator& rhs);

	protected:
		~ChannelIterator();

		virtual bool Input(const wchar_t * name, IData * data) override;
	};
}

