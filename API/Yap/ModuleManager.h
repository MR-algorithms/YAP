#ifndef ModuleManager_h__20160813
#define ModuleManager_h__20160813

#pragma once
#include "ProcessorManagerAgent.h"

#include <map>
#include <memory>

#include "Interface/IProcessor.h"

namespace Yap
{
	class ModuleManager : public IProcessorContainer
	{
		typedef std::map<std::wstring, std::shared_ptr<ModuleAgent>> ModuleContainer;
		typedef ModuleContainer::iterator ModuleIter;

		class ProcessorIterator : public IProcessorIter
		{
			explicit ProcessorIterator(ModuleManager& manager);
			// Inherited via IIterator
			virtual IProcessor * GetFirst() override;
			virtual IProcessor * GetNext() override;

			ModuleManager& _manager;

			ModuleIter _current_module;
			std::shared_ptr<IProcessorIter> _current_module_processors;

			friend class ModuleManager;
		};

	public:
		ModuleManager();
		~ModuleManager();


		virtual IProcessor * Find(const wchar_t * name) override;
		virtual IProcessorIter * GetIterator() override;

		bool LoadModule(const wchar_t * module_path);

		IProcessor * FindProcessorInAllModules(const wchar_t * name);
		IProcessor * CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id);

		void Reset();
	protected:
		ModuleContainer _modules;

	friend class PipelineConstructor;
	};
}
#endif // ModuleManager_h__
