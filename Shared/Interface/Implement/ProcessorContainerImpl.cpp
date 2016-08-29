#include "ProcessorContainerImpl.h"
#include <cassert>

using namespace Yap;

ProcessorContainerImpl::ProcessorContainerImpl()
{
}


ProcessorContainerImpl::~ProcessorContainerImpl()
{
	for (auto processor : _processors)
	{
		delete processor.second;
	}
}

IProcessor * ProcessorContainerImpl::Find(const wchar_t * name)
{
	auto iter = _processors.find(name);
	return  (iter != _processors.end()) ? iter->second : nullptr;
}

IProcessorIter * Yap::ProcessorContainerImpl::GetIterator()
{
	try
	{
		return new ProcessorIterator(*this);
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

bool ProcessorContainerImpl::AddProcessor(IProcessor * processor)
{
	assert(processor != nullptr);
	assert(_processors.find(processor->GetClassId()) == _processors.end());

	_processors.insert(std::make_pair(processor->GetClassId(), processor));

	return true;
}

void Yap::ProcessorContainerImpl::DeleteThis()
{
	for (auto item : _processors)
	{
		item.second->DeleteThis();
	}
	_processors.clear();
}

Yap::ProcessorContainerImpl::ProcessorIterator::ProcessorIterator(ProcessorContainerImpl & container) :
	_container(container)
{
}

IProcessor * Yap::ProcessorContainerImpl::ProcessorIterator::GetFirst()
{
	_current = _container._processors.begin();

	return  (_current != _container._processors.end()) ? _current->second : nullptr;
}

IProcessor * Yap::ProcessorContainerImpl::ProcessorIterator::GetNext()
{
	return  (_current != _container._processors.end() && ++_current != _container._processors.end()) ?
		_current->second : nullptr;
}
