#include "CompositeProcessor.h"

#include <utility>

using namespace Yap;
using namespace std;

CompositeProcessor::CompositeProcessor(const wchar_t * class_id) :
	ProcessorImpl(class_id)
{
}

CompositeProcessor::~CompositeProcessor()
{
}

Yap::CompositeProcessor::CompositeProcessor(CompositeProcessor& rhs) :
	ProcessorImpl(rhs.GetClassId())
{

}

Yap::IProcessor * Yap::CompositeProcessor::Clone()
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool Yap::CompositeProcessor::UpdateProperties(IPropertyContainer * params)
{
	for (auto processor : _processors)
	{
		if (!processor.second->UpdateProperties(params))
			return false;
	}

	return true;
}

bool Yap::CompositeProcessor::Link(const wchar_t * output, 
	IProcessor * next, 
	const wchar_t * next_input)
{
	auto iter = _output.find(output);
	if (iter == _output.end())
		return false;

	return iter->second.processor->Link(iter->second.port.c_str(), next, next_input);
}

bool Yap::CompositeProcessor::Input(const wchar_t * port, IData * data)
{
	auto iter = _inputs.find(port);
	if (iter == _inputs.end())
		return false;

	return iter->second.processor->Input(iter->second.port.c_str(), data);
}

bool Yap::CompositeProcessor::MapInput(const wchar_t * port, 
	const wchar_t * inner_processor, 
	const wchar_t * inner_port)
{
	if (_inputs.find(port) != _inputs.end())
		return false;

	auto processor = Find(inner_processor);
	if (!processor)
		return false;

	_inputs.insert(make_pair(port, Anchor(processor, inner_port)));
	return true;
}

bool Yap::CompositeProcessor::MapOutput(const wchar_t * port, 
	const wchar_t * inner_processor, 
	const wchar_t * inner_port)
{
	if (_output.find(port) == _output.end())
		return false;

	auto processor = Find(inner_processor);
	if (!processor)
		return false;

	_output.insert(make_pair(port, Anchor(processor, inner_port)));
	return true;
}

bool Yap::CompositeProcessor::AddProcessor(IProcessor * processor)
{
	assert(processor);
	if (_processors.find(processor->GetInstanceId()) != _processors.end())
		return false;

	_processors.insert(make_pair(processor->GetInstanceId(), YapShared(processor)));

	return true;
}

IProcessor * Yap::CompositeProcessor::Find(const wchar_t * instance_id)
{
	auto iter = _processors.find(instance_id);
	if (iter == _processors.end())
		return nullptr;

	return iter->second.get();
}

