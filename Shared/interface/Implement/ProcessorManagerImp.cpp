#include "ProcessorManagerImp.h"
#include <cassert>

using namespace Yap;

CProcessorManagerImp::CProcessorManagerImp()
{
}


CProcessorManagerImp::~CProcessorManagerImp()
{
	for (auto processor : _processors)
	{
		delete processor.second;
	}
}

IProcessor * CProcessorManagerImp::GetFirstProcessor()
{
	_current = _processors.begin();

	return  (_current != _processors.end()) ? _current->second : nullptr;
}

IProcessor * CProcessorManagerImp::GetNextProcessor()
{
	return  (++_current != _processors.end()) ? _current->second : nullptr;
}

IProcessor * CProcessorManagerImp::GetProcessor(const wchar_t * name)
{
	auto iter = _processors.find(name);
	return  (iter != _processors.end()) ? iter->second : nullptr;
}

bool CProcessorManagerImp::AddProcessor(IProcessor * processor)
{
	assert(processor != nullptr);
	assert(_processors.find(processor->GetClassId()) == _processors.end());

	_processors.insert(std::make_pair(processor->GetClassId(), processor));

	return true;
}

void Yap::CProcessorManagerImp::Delete()
{
	for (auto item : _processors)
	{
		item.second->Delete();
	}
	_processors.clear();
	_current = _processors.begin();
}
