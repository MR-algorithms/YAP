#pragma once
#include "Interface/Implement/ProcessorImpl.h"
namespace Yap
{
	class ChannelIterator :
		public ProcessorImpl
	{
	public:
		ChannelIterator();

		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;

	protected:
		~ChannelIterator();
	};
}

