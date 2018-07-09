#pragma once
#ifndef FolderCollector_h__20180117
#define FolderCollector_h__20180117

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class FolderCollector : public ProcessorImpl
	{
		IMPLEMENT_SHARED(FolderCollector)
	public:
		FolderCollector();
		FolderCollector(const FolderCollector&);

		virtual bool Input(const wchar_t * name, IData * data) override;

	private:
		~FolderCollector();

		std::map<unsigned int, SmartPtr<FloatData>> _collector;
		unsigned int _count = 0;

	};
}
#endif // !FolderCollector_h__20180117
