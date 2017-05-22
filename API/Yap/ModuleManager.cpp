#include "ModuleManager.h"
#include "ModuleAgent.h"

#include <utility>
#include <vector>
#include "Implement\LogImpl.h"
#include "Interface\Interfaces.h"
#include "Interface/SmartPtr.h"

using namespace Yap;
using namespace std;

std::map <std::wstring, Yap::SmartPtr<IProcessor>> ProcessorManager::s_processors;
std::multimap<std::wstring, std::wstring> ProcessorManager::s_short_to_full_id;

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

ModuleManager::ModuleManager() :
	_processor_manager{make_shared<ProcessorManager>()}
{
}


ModuleManager::~ModuleManager()
{
	_modules.clear();
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
		auto full_id = s_short_to_full_id.equal_range(name);

		// if one and only one full qualified id has this short form
//		if (full_id.first != full_id.second)
		if (s_short_to_full_id.count(name) != 1)
			return nullptr;

		if (full_id.first != s_short_to_full_id.end())
		{
			id_string = full_id.first->second;
		}
	}

	auto iter = s_processors.find(id_string.c_str());
	return (iter != s_processors.end()) ? iter->second.get() : nullptr;
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
//    assert( 0 && "Don't use this function. Use RegisterProcessor() instead.");
//    return false;
	return RegisterProcessor(name, processor);
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
    s_processors.clear();
	s_short_to_full_id.clear();
}

bool ProcessorManager::RegisterProcessor(const wchar_t *name, IProcessor * processor)
{
    if (s_processors.find(name) != s_processors.end())
        return false;

	wstring id_string(name);
	s_processors.emplace(id_string, YapShared(processor));
	auto separator_pos = id_string.find(L"::");
	if (separator_pos != wstring::npos)
	{
		auto short_id = id_string.substr(separator_pos + 2);
		s_short_to_full_id.emplace(short_id, id_string);
	}

	return true;
}

Yap::ProcessorManager::ProcessorIterator::ProcessorIterator(ProcessorManager & manager) :
	_manager(manager), _current{manager.s_processors.end()}
{
}

IProcessor * Yap::ProcessorManager::ProcessorIterator::GetFirst()
{
	_current = _manager.s_processors.begin();

	return (_manager.s_processors.empty() ? nullptr : _current->second.get());
}

IProcessor * Yap::ProcessorManager::ProcessorIterator::GetNext()
{
	if (_current == _manager.s_processors.end() || ++_current == _manager.s_processors.end())
		return nullptr;

	return _current->second.get();
}

bool Yap::Module::Load(const wchar_t * plugin_path, IProcessorContainer& containers)
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
		containers.Add((module_name + L"::" + processor->GetClassId()).c_str(), processor);
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
		auto result = _modules.emplace(module_path, YapShared(new Module()));
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
