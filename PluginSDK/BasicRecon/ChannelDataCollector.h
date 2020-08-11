#pragma once
#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class ChannelDataCollector :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(ChannelDataCollector)
	public:
		ChannelDataCollector(void);
		ChannelDataCollector(const ChannelDataCollector& rhs);

	protected:
		~ChannelDataCollector(void);

		virtual bool Input(const wchar_t * name, IData * data) override;
		struct CollectorBuffer
		{
			SmartPtr<ComplexFloatData> buffer;
			unsigned int count;

			CollectorBuffer() : count(0) {}
			CollectorBuffer(CollectorBuffer& rhs) : count(rhs.count), buffer(rhs.buffer) {}
			CollectorBuffer(CollectorBuffer&& rhs) : count(rhs.count), buffer(rhs.buffer) {}

			~CollectorBuffer() {}
		};

		std::vector<unsigned int> GetKey(IDimensions * dimensions);
		std::map<std::vector<unsigned int>, CollectorBuffer> _collector_buffers;
	};
}
