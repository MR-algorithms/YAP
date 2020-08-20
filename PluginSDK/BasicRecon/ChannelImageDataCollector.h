#pragma once
#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class ChannelImageDataCollector :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(ChannelImageDataCollector)
	public:
		ChannelImageDataCollector(void);
		ChannelImageDataCollector(const ChannelImageDataCollector& rhs);

	protected:
		~ChannelImageDataCollector(void);

		virtual bool Input(const wchar_t * name, IData * data) override;

		void Log(int channel_index, int slice_index, int ready_phasesteps);
		
		struct CollectorBuffer
		{
			SmartPtr<FloatData> buffer;
			unsigned int count;
			int ready_phasesteps;

			CollectorBuffer() : count(0), ready_phasesteps(-1) {}
			CollectorBuffer(CollectorBuffer& rhs) : count(rhs.count), ready_phasesteps(rhs.ready_phasesteps), buffer(rhs.buffer) {}
			CollectorBuffer(CollectorBuffer&& rhs) : count(rhs.count), ready_phasesteps(rhs.ready_phasesteps), buffer(rhs.buffer) {}

			~CollectorBuffer() {}
		};

		std::vector<unsigned int> GetKey(IDimensions * dimensions);
		std::map<std::vector<unsigned int>, CollectorBuffer> _collector_buffers;
		
	};
}
