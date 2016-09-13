#pragma once
#include "Interface/Implement/ProcessorImpl.h"
namespace Yap
{
	class ChannelIterator :
		public ProcessorImpl
	{
	public:
		ChannelIterator();
		virtual ~ChannelIterator();

		virtual bool OnInit() override;

		virtual IProcessor * Clone() override;

		virtual bool Input(const wchar_t * name, IData * data) override;

	};
}

