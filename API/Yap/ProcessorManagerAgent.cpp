#include "ProcessorManagerAgent.h"
#include <assert.h>


using namespace Yap;

CProcessorManagerAgent::CProcessorManagerAgent() :
	_module(0),
	_manager(nullptr)
{
}


CProcessorManagerAgent::~CProcessorManagerAgent()
{
	_manager.reset();
	if (_module)
	{
		::FreeModule(_module);
	}
}

bool Yap::CProcessorManagerAgent::Load(const wchar_t * plugin_path)
{
	if (_module != 0)
	{
		return true;
	}

	_module = ::LoadLibrary(plugin_path);

	if (_module == 0)
	{
		return false;
	}

	auto create_func = (Yap::IProcessorManager*(*)())::GetProcAddress(
		_module, "GetProcessorManager");
	if (create_func == nullptr)
	{
		::FreeLibrary(_module);
		_module = 0;
		return false;
	}

	_manager = SmartPtr<IProcessorManager>(create_func());
	if (!_manager)
	{
		::FreeLibrary(_module);
		_module = 0;
		return false;
	}

	return true;
}

Yap::IProcessor * Yap::CProcessorManagerAgent::GetFirstProcessor()
{
	assert(_manager);
	return _manager ? _manager->GetFirstProcessor() : nullptr;
}

Yap::IProcessor * Yap::CProcessorManagerAgent::GetNextProcessor()
{
	assert(_manager);
	return _manager ? _manager->GetNextProcessor() : nullptr;
}

Yap::IProcessor * Yap::CProcessorManagerAgent::GetProcessor(const wchar_t * name)
{
	assert(_manager);
	return _manager ? _manager->GetProcessor(name) : nullptr;
}
