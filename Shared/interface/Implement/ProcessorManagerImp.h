#ifndef ProcessorManagerImp_h__20160813
#define ProcessorManagerImp_h__20160813

#pragma once

#include "Interface/IProcessor.h"
#include <map>

namespace Yap
{
	class CProcessorManagerImp :
		public IProcessorManager, public IDynamicObject
	{
	public:
		CProcessorManagerImp();
		~CProcessorManagerImp();

		virtual IProcessor * GetFirstProcessor() override;
		virtual IProcessor * GetNextProcessor() override;
		virtual IProcessor * GetProcessor(const wchar_t * name) override;
		virtual void Delete() override;

		bool AddProcessor(IProcessor * processor);

	private:
		std::map<std::wstring, IProcessor*> _processors;
		std::map<std::wstring, IProcessor*>::iterator _current;
	};
}
#endif // ProcessorManagerImp_h__
