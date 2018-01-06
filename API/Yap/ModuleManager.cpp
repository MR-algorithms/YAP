#include "ModuleManager.h"
#include "ModuleAgent.h"

#include <utility>
#include <vector>
#include "Implement\LogImpl.h"
#include "Interface\Interfaces.h"
#include "Interface/SmartPtr.h"
#include "Implement\PythonImpl.h"

using namespace Yap;
using namespace std;

std::map <std::wstring, Yap::SmartPtr<IProcessor>> ImportedProcessorManager::s_registered_processors;

std::wstring GetFileNameFromPath(const wchar_t * path)
{
	wstring path_string(path);
	auto start_pos = path_string.rfind(L'\\');
	auto end_pos = path_string.rfind(L'.');
	if (end_pos == wstring::npos)
	{
		end_pos = path_string.length();
	}

	return path_string.substr(start_pos + 1, end_pos - start_pos - 1);
}

shared_ptr<ModuleManager> ModuleManager::s_instance;

ModuleManager& Yap::ModuleManager::GetInstance()
{
	if (!s_instance)
	{
		try
		{
			s_instance = shared_ptr<ModuleManager>(new ModuleManager);
		}
		catch (bad_alloc&)
		{
			assert(0 && L"Failed new ModuleManager.");
		}
	}
	return *s_instance;
}

ModuleManager::ModuleManager() :
	_imported_processors{YapShared(new ImportedProcessorManager())}
{
}

ModuleManager::~ModuleManager()
{
}

void ModuleManager::Release()
{
	_imported_processors.reset();
	for (auto module : _modules)
	{
		module.second->Release();
	}

	_modules.clear();
}

ImportedProcessorManager::ImportedProcessorManager()
{
	Reset();
}

IProcessor * ImportedProcessorManager::Find(const wchar_t * name)
{
	// Processor class id may take one of the following two forms: 
	// 1. BasicRecon::ZeroFilling, where BasicRecon is the module name;
	// 2. ZeroFilling

	wstring id_string(name);
	auto pos = id_string.find(L"::");
	if (pos == wstring::npos)
	{
		// if one and only one full qualified id has this short form
        if (_short_to_full_id.count(name) > 1)
			return nullptr;
		
		auto full_id = _short_to_full_id.find(name);
		if (full_id != _short_to_full_id.end())
		{
			id_string = full_id->second;
		}
	}

	auto iter = _processors.find(id_string.c_str());
	return (iter != _processors.end()) ? iter->second.get() : nullptr;
}

IProcessorIter * Yap::ImportedProcessorManager::GetIterator()
{
	try
	{
		return new ProcessorIterator(*this);
	}
	catch (bad_alloc&)
	{
		return nullptr;
	}
}

bool Yap::ImportedProcessorManager::Add(const wchar_t * name, IProcessor * processor)
{
	if (_processors.find(name) != _processors.end())
		return false;

	wstring id_string(name);
	_processors.emplace(id_string, YapShared(processor));
	auto separator_pos = id_string.find(L"::");
	if (separator_pos != wstring::npos)
	{
		auto short_id = id_string.substr(separator_pos + 2);
		_short_to_full_id.emplace(short_id, id_string);
	}

	return true;
}

bool Yap::ImportedProcessorManager::Delete(const wchar_t * name)
{
	auto iter = _processors.find(name);
	if (iter != _processors.end())
	{
		_processors.erase(iter);
		return true;
	}

	wstring id_string(name);
	auto separator_pos = id_string.find(L"::");
	if (separator_pos != wstring::npos)
	{
		auto short_id = id_string.substr(separator_pos + 2);
		auto iter2 = _short_to_full_id.find(short_id);
		if (iter2 != _short_to_full_id.end())
		{
			_short_to_full_id.erase(iter2);
		}
	}

	return false;
}

void Yap::ImportedProcessorManager::Clear() 
{
	_processors.clear();
	_short_to_full_id.clear();
}

Yap::IProcessor * Yap::ImportedProcessorManager::CreateProcessor(const wchar_t * class_id, 
													  const wchar_t * instance_id)
{
	auto processor = Find(class_id);
	if (processor == nullptr)
		return nullptr;

	auto new_processor = dynamic_cast<IProcessor*>(processor->Clone());
	if (new_processor != nullptr)
	{
		new_processor->SetInstanceId(instance_id);
	}

	return new_processor;
}

void Yap::ImportedProcessorManager::Reset()
{
    _processors.clear();
	_short_to_full_id.clear();
	for (auto processor : s_registered_processors)
	{
		Add(processor.first.c_str(), processor.second.get());
	}
}

bool ImportedProcessorManager::RegisterProcessor(const wchar_t *name, IProcessor * processor)
{
    if (s_registered_processors.find(name) != s_registered_processors.end())
        return false;

	s_registered_processors.emplace(name, YapShared(processor));

	return true;
}

Yap::ImportedProcessorManager::ProcessorIterator::ProcessorIterator(ImportedProcessorManager & manager) :
	_manager(manager), _current{manager._processors.end()}
{
}

IProcessor * Yap::ImportedProcessorManager::ProcessorIterator::GetFirst()
{
	_current = _manager._processors.begin();

	return (_manager._processors.empty() ? nullptr : _current->second.get());
}

IProcessor * Yap::ImportedProcessorManager::ProcessorIterator::GetNext()
{
	if (_current == _manager._processors.end() || ++_current == _manager._processors.end())
		return nullptr;

	return _current->second.get();
}

void Yap::ImportedProcessorManager::ProcessorIterator::DeleteThis()
{
	delete this;
}

bool Yap::Module::Load(const wchar_t * plugin_path, IProcessorContainer& imported_processors)
{
	if (_module == 0)
	{
		_module = ::LoadLibrary(plugin_path);

		if (_module == 0)
		{
			return false;
		}

		auto log_func = (Yap::ILogUser*(*)()) ::GetProcAddress(_module, "GetLogUser");
		if (log_func != nullptr)
		{
			auto log_user = log_func();
			if (log_user != nullptr)
			{
				log_user->SetLog(&LogImpl::GetInstance());
				LogImpl::GetInstance().AddUser(log_user);
			}
		}

		auto python_func = (Yap::IPythonUser*(*)()) ::GetProcAddress(_module, "GetPythonUser");
		if (python_func != nullptr)
		{
			auto python_user = python_func();
			if (python_user != nullptr)
			{
				python_user->SetPython(PythonFactory::GetPython());
			}
		}
	}

	auto create_func = (Yap::IProcessorContainer*(*)())::GetProcAddress(
		_module, "GetProcessorManager");
	if (create_func == nullptr)
	{
		::FreeLibrary(_module);
		_module = 0;
		return false;
	}

	auto processor_manager = YapShared(create_func());
	if (!processor_manager)
	{
		::FreeLibrary(_module);
		_module = 0;
		return false;
	}

	auto source_iter = processor_manager->GetIterator();
	auto module_name = GetModuleNameFromPath(plugin_path);
	for (auto processor = source_iter->GetFirst(); processor != nullptr; processor = source_iter->GetNext())
	{
		auto imported_processor = dynamic_cast<IProcessor*>(processor->Clone());
		imported_processor->SetModule(this);
		imported_processors.Add((module_name + L"::" + imported_processor->GetClassId()).c_str(),
			imported_processor);
	}

	Lock();

	return true;
}

std::wstring Yap::Module::GetModuleNameFromPath(const wchar_t * path)
{
	wstring module_string(path);
	auto separator_pos = module_string.find_last_of(L"\\/");
	if (separator_pos != wstring::npos)
	{
		module_string = module_string.substr(separator_pos + 1);
	}

	auto dot_pos = module_string.find(L'.');
	if (dot_pos != wstring::npos)
	{
		module_string = module_string.substr(0, dot_pos);
	}

	return module_string;
}

bool Yap::ModuleManager::LoadModule(const wchar_t * module_path)
{
	auto iter = _modules.find(module_path);
	if (iter == _modules.end())
	{
		auto module = make_pair(module_path, make_shared<Module>());
		if (!module.second->Load(module_path, *_imported_processors))
			return false;

		_modules.insert(module);
		return true;
	}
	else
	{
		return iter->second->Load(module_path, *_imported_processors);
	}
}

void Yap::ModuleManager::Reset()
{
	assert(_imported_processors);

	_imported_processors->Reset();
}

IProcessor * Yap::ModuleManager::CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id)
{
	assert(_imported_processors);
	return _imported_processors->CreateProcessor(class_id, instance_id);
}

ImportedProcessorManager& Yap::ModuleManager::GetProcessorManager()
{
	assert(_imported_processors);
	return *_imported_processors;
}
