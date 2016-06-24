#include "stdafx.h"
#include "ProcessorManager.h"
#include <cassert>

using namespace Yap;

CProcessorManager::CProcessorManager()
{
}


CProcessorManager::~CProcessorManager()
{
	for (auto processor : _processors)
	{
		delete processor.second;
	}
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

IProcessor * CProcessorManager::GetProcessor(const wchar_t * name)
{
	auto iter = _processors.find(name);
	return  (iter != _processors.end()) ? iter->second : nullptr;
}

bool CProcessorManager::AddProcessor(IProcessor * processor)
{
	assert(processor != nullptr);
	assert(_processors.find(processor->GetClassId()) == _processors.end());

	_processors.insert(std::make_pair(processor->GetClassId(), processor));

	return true;
}
