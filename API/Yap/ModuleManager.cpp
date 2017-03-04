#include "ModuleManager.h"
#include "ProcessorManagerAgent.h"
#include "Interface/IMemory.h"

#include <utility>
#include <vector>

using namespace Yap;
using namespace std;

std::map <std::wstring, Yap::SmartPtr<IProcessor>> ModuleManager::s_processors;

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

ModuleManager::ModuleManager()
{
}


ModuleManager::~ModuleManager()
{
}

Yap::IProcessor * Yap::ModuleManager::Find(const wchar_t * name)
{
	// BasicRecon::ZeroFilling
	wstring name_string(name);
	auto pos = name_string.find(L"::");
	if (pos != wstring::npos)
	{
		auto iter = _modules.find(name_string.substr(0, pos));
		if (iter == _modules.end())
			return nullptr;

		return iter->second->Find(name_string.substr(pos + 2).c_str());
	}
	else
	{
		return FindProcessorInAllModules(name);
	}
}

IProcessorIter * Yap::ModuleManager::GetIterator()
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

bool Yap::ModuleManager::Add(const wchar_t * /*name*/, IProcessor * /*processor*/)
{
    assert( 0 && "Don't use this function. Use RegisterProcessor() instead.");
    return false;
}

Yap::IProcessor * Yap::ModuleManager::FindProcessorInAllModules(const wchar_t * name)
{
	vector<IProcessor*> processors;

    auto iter = s_processors.find(name);
    if (iter != s_processors.end())
    {
        processors.push_back(iter->second.get());
    }

	for (auto& module : _modules)
	{
		auto processor = module.second->Find(name);
		if (processor != nullptr)
		{
			processors.push_back(processor);
		}
	}

	if (processors.empty())
		return nullptr;

	if (processors.size() > 1)
		return nullptr;

	return processors[0];
}

Yap::IProcessor * Yap::ModuleManager::CreateProcessor(const wchar_t * class_id, 
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

void Yap::ModuleManager::Reset()
{
    _modules.clear();
}

bool ModuleManager::RegisterProcessor(const wchar_t *name, IProcessor * processor)
{
    if (s_processors.find(name) != s_processors.end())
        return false;

    s_processors.insert(make_pair(name, YapShared(processor)));
    return true;

}

bool Yap::ModuleManager::LoadModule(const wchar_t * module_path)
{
	shared_ptr<ModuleAgent> processor_manager_agent(new ModuleAgent);
	if (!processor_manager_agent->Load(module_path))
		return false;

	_modules.insert(make_pair(GetFileNameFromPath(module_path), processor_manager_agent));

	return true;
}

Yap::ModuleManager::ProcessorIterator::ProcessorIterator(ModuleManager & manager) :
	_manager(manager),
	_current_module(_manager._modules.end())
{
}

IProcessor * Yap::ModuleManager::ProcessorIterator::GetFirst()
{
	_current_module = _manager._modules.begin();
	if (_current_module != _manager._modules.end())
	{
		_current_module_processors = YapDynamic(_current_module->second->GetIterator());
		return _current_module_processors ? _current_module_processors->GetFirst() : nullptr;
	}
	else
	{
		return nullptr;
	}
}

IProcessor * Yap::ModuleManager::ProcessorIterator::GetNext()
{
	if (_current_module == _manager._modules.end())
		return nullptr;

	assert(_current_module_processors && 
		   "IProcessorIter for the current module should always be available.");

	if (!_current_module_processors)
		return nullptr;

	auto processor = _current_module_processors->GetNext();
	if (processor != nullptr)
	{
		return processor;
	}
	else // end of the current module.
	{
		if (++_current_module != _manager._modules.end())
		{
			assert(_current_module->second != nullptr && 
					"nullptr should not have never been added to module containers.");

			_current_module_processors = YapDynamic(_current_module->second->GetIterator());

			return _current_module_processors ? _current_module_processors->GetFirst() : nullptr;
		}
		else
		{
			return nullptr;
		}
	}
}
