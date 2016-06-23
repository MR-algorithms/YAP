#pragma once

#include "../Interface/YapInterfaces.h"
#include <map>

namespace Yap
{
	class CProcessorManager :
		public IProcessorManager
	{
	public:
		CProcessorManager();
		~CProcessorManager();

		virtual IProcessor * GetFirstProcessor() override;
		virtual IProcessor * GetNextProcessor() override;
		virtual IProcessor * GetProcessor(const wchar_t * name) override;

		bool AddProcessor(IProcessor * processor);

	private:
		std::map<std::wstring, IProcessor*> _processors;
		std::map<std::wstring, IProcessor*>::iterator _current;
	};
}
