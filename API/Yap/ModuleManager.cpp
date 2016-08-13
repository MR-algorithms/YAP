#include "ModuleManager.h"
#include "ProcessorManagerAgent.h"
#include <utility>
#include <vector>

using namespace Yap;
using namespace std;

std::wstring GetFileNameFromPath(const wchar_t * path)
{
	wstring path_string(path);
	auto start_pos = path_string.rfind(L'\\');
	auto end_pos = path_string.rfind(L'.');
	if (end_pos == wstring::npos)
	{
		end_pos = path_string.length();
	}

	return path_string.substr(start_pos, end_pos - start_pos - 1);
}

CModuleManager::CModuleManager()
{
}


CModuleManager::~CModuleManager()
{
	for (auto module : _modules)
	{
		module.second->Release();
	}
	_modules.clear();
}

Yap::IProcessor * Yap::CModuleManager::GetFirstProcessor()
{
	auto _current_module = _modules.begin();
	if (!_modules.empty())
	{
		return _current_module->second->GetFirstProcessor();
	}
	else
	{
		return nullptr;
	}
}

Yap::IProcessor * Yap::CModuleManager::GetNextProcessor()
{
	if (_current_module == _modules.end())
		return nullptr;

	auto processor = _current_module->second->GetNextProcessor();
	if (processor == nullptr)
	{
		if (++_current_module == _modules.end())
			return nullptr;

		processor = _current_module->second->GetFirstProcessor();
	}

	return processor;
}

Yap::IProcessor * Yap::CModuleManager::GetProcessor(const wchar_t * name)
{
	// BasicRecon::ZeroFilling
	wstring name_string(name);
	auto pos = name_string.find(L"::");
	if (pos != wstring::npos)
	{
		auto iter = _modules.find(name_string.substr(0, pos));
		if (iter == _modules.end())
			return nullptr;

		return iter->second->GetProcessor(name_string.substr(pos + 2).c_str());
	}
	else
	{
		return FindProcessorInAllModules(name);
	}
}

Yap::IProcessor * Yap::CModuleManager::FindProcessorInAllModules(const wchar_t * name)
{
	vector<IProcessor*> processors;
	for (auto& module : _modules)
	{
		auto processor = module.second->GetProcessor(name);
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

Yap::IProcessor * Yap::CModuleManager::CreateProcessor(const wchar_t * class_id, const wchar_t * instance_id)
{
	auto processor = GetProcessor(class_id);
	if (processor == nullptr)
		return nullptr;

	auto new_processor = processor->Clone();
	if (new_processor != nullptr)
	{
		new_processor->SetInstanceId(instance_id);
	}

	return new_processor;
}

void Yap::CModuleManager::Reset()
{
	_modules.clear();
	_current_module = _modules.begin();
}

bool Yap::CModuleManager::LoadModule(const wchar_t * module_path)
{
	shared_ptr<CProcessorManagerAgent> processor_manager_agent(new CProcessorManagerAgent);
	if (!processor_manager_agent->Load(module_path))
		return false;

	_modules.insert(make_pair(GetFileNameFromPath(module_path), processor_manager_agent));

	return true;
}

