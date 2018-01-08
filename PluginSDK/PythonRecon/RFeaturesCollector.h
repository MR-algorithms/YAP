//////////////////////////////////////////////////////////////////////////
/* Radiomics Features Collector */
#pragma once
#ifndef RadiomicsFeaturesCollector_h__20171229
#define RadiomicsFeaturesCollector_h__20171229

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class RFeaturesCollector : public ProcessorImpl
	{
		IMPLEMENT_SHARED(RFeaturesCollector)
	public:
		RFeaturesCollector();
		RFeaturesCollector(const RFeaturesCollector&);

		virtual bool Input(const wchar_t * name, IData * data) override;

	private:
		~RFeaturesCollector();

		struct CollectorBuffer
		{
			SmartPtr<DoubleData> buffer;
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
#endif // !RadiomicsFeaturesCollector_h__20171229
