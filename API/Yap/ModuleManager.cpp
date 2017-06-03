#include "ModuleManager.h"
#include "ModuleAgent.h"

#include <utility>
#include <vector>
#include "Implement\LogImpl.h"
#include "Interface\Interfaces.h"
#include "Interface/SmartPtr.h"

using namespace Yap;
using namespace std;

std::map <std::wstring, Yap::SmartPtr<IProcessor>> ProcessorManager::s_registered_processors;

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
	_processor_manager{YapShared(new ProcessorManager())}
{
}

ModuleManager::~ModuleManager()
{
	_processor_manager.reset();
	_modules.clear();
}

ProcessorManager::ProcessorManager()
{
	Reset();
}

IProcessor * ProcessorManager::Find(const wchar_t * name)
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

IProcessorIter * Yap::ProcessorManager::GetIterator()
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

bool Yap::ProcessorManager::Add(const wchar_t * name, IProcessor * processor)
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


Yap::IProcessor * Yap::ProcessorManager::CreateProcessor(const wchar_t * class_id, 
													  const wchar_t * instance_id)
{
	auto processor = Find(class_id);
	if (processor == nullptr)
		return nullptr;

	auto new_processor = processor->Clone();
	if (new_processor != nullptr)
	{
		new_processor->SetInstanceId(instance_id);
	}

	return new_processor;
}

void Yap::ProcessorManager::Reset()
{
    _processors.clear();
	_short_to_full_id.clear();
	for (auto processor : s_registered_processors)
	{
		Add(processor.first.c_str(), processor.second.get());
	}
}

bool ProcessorManager::RegisterProcessor(const wchar_t *name, IProcessor * processor)
{
    if (s_registered_processors.find(name) != s_registered_processors.end())
        return false;

	s_registered_processors.emplace(name, YapShared(processor));

	return true;
}

Yap::ProcessorManager::ProcessorIterator::ProcessorIterator(ProcessorManager & manager) :
	_manager(manager), _current{manager._processors.end()}
{
}

IProcessor * Yap::ProcessorManager::ProcessorIterator::GetFirst()
{
	_current = _manager._processors.begin();

	return (_manager._processors.empty() ? nullptr : _current->second.get());
}

IProcessor * Yap::ProcessorManager::ProcessorIterator::GetNext()
{
	if (_current == _manager._processors.end() || ++_current == _manager._processors.end())
		return nullptr;

	return _current->second.get();
}

bool Yap::Module::Load(const wchar_t * plugin_path, IProcessorContainer& processors)
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
				LogImpl::GetInstance().AddUser(log_user);
				log_user->SetLog(&LogImpl::GetInstance());
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

	auto dll_processor_manager = YapShared(create_func());
	if (!dll_processor_manager)
	{
		::FreeLibrary(_module);
		_module = 0;
		return false;
	}

	auto source_iter = dll_processor_manager->GetIterator();
	auto module_name = GetModuleNameFromPath(plugin_path);
	for (auto processor = source_iter->GetFirst(); processor != nullptr; processor = source_iter->GetNext())
	{
		processor->SetModule(this);
		processors.Add((module_name + L"::" + processor->GetClassId()).c_str(), processor);
	}

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
		auto result = _modules.emplace(module_path, make_shared<Module>());
		if (!result.second)
			return false;

		iter = result.first;
	}

	return iter->second->Load(module_path, *_processor_manager);
}

void Yap::ModuleManager::Reset()
{
	assert(_processor_manager);

	_processor_manager->Reset();
}

IProcessor * Yap::ModuleManager::CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id)
{
	assert(_processor_manager);
	return _processor_manager->CreateProcessor(class_id, instance_id);
}

ProcessorManager& Yap::ModuleManager::GetProcessorManager()
{
	assert(_processor_manager);
	return *_processor_manager;
}
