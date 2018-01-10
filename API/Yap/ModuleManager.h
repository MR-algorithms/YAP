#ifndef ModuleManager_h__20160813
#define ModuleManager_h__20160813

#pragma once

#include <map>
#include <memory>
#include "Interface\smartptr.h"
#include "Interface\IProcessor.h"
#include <windows.h>
#include "Interface\IPython.h"

namespace Yap
{
	class ImportedProcessorManager : public IProcessorContainer
	{
		IMPLEMENT_SHARED(ImportedProcessorManager)

	public:
		typedef std::map <std::wstring, Yap::SmartPtr<IProcessor>> Processors;
		typedef Processors::iterator ProcessorIter;

		class ProcessorIterator : public IProcessorIter
		{
			explicit ProcessorIterator(ImportedProcessorManager& manager);
			// Inherited via IIterator
			virtual IProcessor * GetFirst() override;
			virtual IProcessor * GetNext() override;
			virtual void DeleteThis() override;

			ImportedProcessorManager& _manager;
			ProcessorIter _current;

			friend class ImportedProcessorManager;
		};

		ImportedProcessorManager();
		virtual IProcessor * Find(const wchar_t * name) override;
		virtual IProcessorIter * GetIterator() override;
		virtual bool Add(const wchar_t * name, IProcessor * processor) override;
		virtual bool Delete(const wchar_t * name) override;
		virtual void Clear() override;

		void Reset();

		IProcessor * CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id);
		static bool RegisterProcessor(const wchar_t * name, IProcessor * procesor);

	private:
		~ImportedProcessorManager() {}
		std::map <std::wstring, Yap::SmartPtr<IProcessor>> _processors;

		/// Used to store processors added directly from application, instead of DLLs.
		static std::map <std::wstring, Yap::SmartPtr<IProcessor>> s_registered_processors;

		std::multimap<std::wstring, std::wstring> _short_to_full_id;

		friend class PipelineConstructor;
	};

	class Module : public ISharedObject
	{
		IMPLEMENT_CLONE(Module)
	public:
		Module() : _use_count{ 0 }, _module{ 0 } {}

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
					auto release_func = (void(*)())::GetProcAddress(
						_module, "ReleaseProcessorManager");
					if (release_func != nullptr)
					{
						release_func();
					}
					::FreeModule(_module);
				}
				_module = 0;
			}
		}

		bool Load(const wchar_t * plugin_path, IProcessorContainer& containers, IPython * python_engine = nullptr);

	private:
		std::wstring GetModuleNameFromPath(const wchar_t * path);

		unsigned int _use_count;

		HINSTANCE _module;
	};

	class ModuleManager
	{
		typedef std::map<std::wstring, std::shared_ptr<Module>> ModuleContainer;
		typedef ModuleContainer::iterator ModuleIter;

	public:
		static ModuleManager& GetInstance();
		~ModuleManager();

		bool LoadModule(const wchar_t * module_path);
		void Reset();
		IProcessor * CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id);
		ImportedProcessorManager& GetProcessorManager();
		IPython * GetPythonEngine();

		HINSTANCE _python_engine_module = 0;

		void Release();
	protected:
		ModuleContainer _modules;
		SmartPtr<ImportedProcessorManager> _imported_processors;

	private:
		ModuleManager();
		static std::shared_ptr<ModuleManager> s_instance;
	};
}
#endif // ModuleManager_h__
