#pragma once
#include "..\Interface\YapInterfaces.h"

#include <map>
#include "..\Interface\SmartPtr.h"
#include "ProcessorManagerAgent.h"
#include <memory>

namespace Yap
{
	class CModuleManager
	{
	public:
		CModuleManager();
		~CModuleManager();

		IProcessor * GetFirstProcessor();

		IProcessor * GetNextProcessor();

		IProcessor * GetProcessor(const wchar_t * name);

		bool LoadModule(const wchar_t * module_path);
		IProcessor * FindProcessorInAllModules(const wchar_t * name);
	protected:
		std::map<std::wstring, std::shared_ptr<CProcessorManagerAgent>> _modules;
		decltype(_modules)::iterator _current_module;
	};
}
