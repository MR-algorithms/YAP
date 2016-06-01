#include "stdafx.h"
#include "ProcessorManager.h"
#include <cassert>


CProcessorManager::CProcessorManager()
{
}


CProcessorManager::~CProcessorManager()
{
}

IProcessor * CProcessorManager::GetFirstProcessor()
{
	_current = _processors.begin();

	return  (_current != _processors.end()) ? _current->second : nullptr;
}

IProcessor * CProcessorManager::GetNextProcessor()
{
	return  (++_current != _processors.end()) ? _current->second : nullptr;
}

IProcessor * CProcessorManager::GetProcessor(const char * name)
{
	auto iter = _processors.find(name);
	return  (iter != _processors.end()) ? iter->second : nullptr;
}

bool CProcessorManager::AddProcessor(IProcessor * processor)
{
	assert(processor != nullptr);
	assert(_processors.find(processor->GetId()) != _processors.end());

	_processors.insert(std::make_pair(processor->GetId(), processor));

	return true;
}
