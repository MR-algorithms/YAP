#include "ProcessorManagerAgent.h"
#include <assert.h>


using namespace Yap;

ModuleAgent::ModuleAgent() :
	_module(0)
{
}


ModuleAgent::~ModuleAgent()
{
	_manager.reset();
	if (_module)
	{
		::FreeModule(_module);
	}
}

bool Yap::ModuleAgent::Load(const wchar_t * plugin_path)
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

	auto create_func = (Yap::IProcessorContainer*(*)())::GetProcAddress(
		_module, "GetProcessorManager");
	if (create_func == nullptr) 
	{
		::FreeLibrary(_module);
		_module = 0;
		return false;
	}

	_manager = YapShared(create_func());
	if (!_manager)
	{
		::FreeLibrary(_module);
		_module = 0;
		return false;
	}

	return true;
}

Yap::IProcessorIter * Yap::ModuleAgent::GetIterator()
{
	assert(_manager);
	return _manager ? _manager->GetIterator() : nullptr;
}

bool Yap::ModuleAgent::Add(const wchar_t * name, IProcessor * processor)
{
	assert(_manager);
	return _manager ? _manager->Add(name, processor) : false;
}

Yap::IProcessor * Yap::ModuleAgent::Find(const wchar_t * name)
{
	assert(_manager);
	return _manager ? _manager->Find(name) : nullptr;
}

