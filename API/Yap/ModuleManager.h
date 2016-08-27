#ifndef ModuleManager_h__20160813
#define ModuleManager_h__20160813

#pragma once
#include "ProcessorManagerAgent.h"

#include <map>
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
		IProcessor * CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id);
		void Reset();
	protected:
		std::map<std::wstring, std::shared_ptr<CProcessorManagerAgent>> _modules;
		decltype(_modules)::iterator _current_module;
	};
}
#endif // ModuleManager_h__
