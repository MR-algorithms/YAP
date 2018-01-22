#pragma once
#ifndef CaseCollector_h__20180117
#define CaseCollector_h__20180117

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class CaseCollector : public ProcessorImpl
	{
		IMPLEMENT_SHARED(CaseCollector)
	public:
		CaseCollector();
		CaseCollector(const CaseCollector&);

		virtual bool Input(const wchar_t * name, IData * data) override;

	private:
		~CaseCollector();

		std::map<unsigned int, SmartPtr<FloatData>> _collector;
		unsigned int _count = 0;

	};
}
#endif // !CaseCollector_h__20180117
