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

		std::map<unsigned int, SmartPtr<FloatData>> _collector;
		unsigned int _count = 0;

	};
}
#endif // !RadiomicsFeaturesCollector_h__20171229
