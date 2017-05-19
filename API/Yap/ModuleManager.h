#ifndef ModuleManager_h__20160813
#define ModuleManager_h__20160813

#pragma once
#include "ModuleAgent.h"

#include <map>
#include <memory>

namespace Yap
{
	class ProcessorManager : public IProcessorContainer
	{
		IMPLEMENT_SHARED(ProcessorManager)

	public:
		typedef std::map <std::wstring, Yap::SmartPtr<IProcessor>> Processors;
		typedef Processors::iterator ProcessorIter;

		class ProcessorIterator : public IProcessorIter
		{
			explicit ProcessorIterator(ProcessorManager& manager);
			// Inherited via IIterator
			virtual IProcessor * GetFirst() override;
			virtual IProcessor * GetNext() override;

			ProcessorManager& _manager;
			ProcessorIter _current;

			friend class ProcessorManager;
		};

		virtual IProcessor * Find(const wchar_t * name) override;
		virtual IProcessorIter * GetIterator() override;
		virtual bool Add(const wchar_t * name, IProcessor * processor) override;

		void Reset();

		IProcessor * CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id);
		static bool RegisterProcessor(const wchar_t * name, IProcessor * procesor);

	private:
		static std::map <std::wstring, Yap::SmartPtr<IProcessor>> s_processors;
		static std::multimap<std::wstring, std::wstring> s_short_to_full_id;

		friend class PipelineConstructor;
	};

	class Module : public ISharedObject
	{
		IMPLEMENT_CLONE(Module)
	public:
		Module() : _use_count{ 0 }, _module{ 0 } {}
		~Module() {}

		virtual void Lock() override
		{
			++_use_count;
		}

		virtual void Release() override
		{
			if (_use_count == 0 || --_use_count == 0)
			{
				if (_module != 0)
				{
					::FreeModule(_module);
				}
				_module = 0;
			}
		}

		bool Load(const wchar_t * plugin_path, IProcessorContainer& containers);

	private:
		std::wstring GetModuleNameFromPath(const wchar_t * path);

		unsigned int _use_count;

		HINSTANCE _module;
		std::wstring _module_name;
	};

	class ModuleManager
	{
		typedef std::map<std::wstring, std::shared_ptr<Module>> ModuleContainer;
		typedef ModuleContainer::iterator ModuleIter;

	public:
		ModuleManager();
		~ModuleManager();

		bool LoadModule(const wchar_t * module_path);
		void Reset();
		IProcessor * CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id);
		ProcessorManager& GetProcessorManager();
	protected:
		ModuleContainer _modules;
		std::shared_ptr<ProcessorManager> _processor_manager;
	};
}
#endif // ModuleManager_h__
