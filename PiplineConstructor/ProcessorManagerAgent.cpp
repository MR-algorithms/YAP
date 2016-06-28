#include "stdafx.h"
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

	_manager = create_func();
	if (_manager == nullptr)
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
	return _manager != nullptr ? _manager->GetFirstProcessor() : nullptr;
}

Yap::IProcessor * Yap::CProcessorManagerAgent::GetNextProcessor()
{
	assert(_manager);
	return _manager != nullptr ? _manager->GetNextProcessor() : nullptr;
}

Yap::IProcessor * Yap::CProcessorManagerAgent::GetProcessor(const wchar_t * name)
{
	assert(_manager);
	return _manager != nullptr ? _manager->GetProcessor(name) : nullptr;
}

void Yap::CProcessorManagerAgent::Release()
{
	assert(_manager);
	_manager->Release();

	delete this;
}
