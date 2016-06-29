#include "stdafx.h"
#include "CompositeProcessor.h"
#include "ProcessorAgent.h"
#include <utility>

using namespace Yap;
using namespace std;

CCompositeProcessor::CCompositeProcessor(const wchar_t * class_id) :
	CProcessorImp(class_id)
{
}


CCompositeProcessor::~CCompositeProcessor()
{
}

Yap::CCompositeProcessor::CCompositeProcessor(CCompositeProcessor& rhs) :
	CProcessorImp(rhs.GetClassId())
{

}

Yap::IProcessor * Yap::CCompositeProcessor::Clone()
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool Yap::CCompositeProcessor::UpdateProperties(IPropertyEnumerator * params)
{
	for (auto processor : _processors)
	{
		if (!processor.second->UpdateProperties(params))
			return false;
	}

	return true;
}

bool Yap::CCompositeProcessor::Link(const wchar_t * output, 
	IProcessor * next, 
	const wchar_t * next_input)
{
	auto iter = _out_ports.find(output);
	if (iter == _out_ports.end())
		return false;

	return iter->second.processor->Link(iter->second.port.c_str(), next, next_input);
}

bool Yap::CCompositeProcessor::Input(const wchar_t * port, IData * data)
{
	auto iter = _in_ports.find(port);
	if (iter == _in_ports.end())
		return false;

	return iter->second.processor->Input(iter->second.port.c_str(), data);
}

bool Yap::CCompositeProcessor::AssignInPort(const wchar_t * port, IProcessor * inner_processor, const wchar_t * inner_port)
{
	if (_in_ports.find(port) != _in_ports.end())
		return false;

	_in_ports.insert(make_pair(port, Anchor(inner_processor, inner_port)));
	return true;
}

bool Yap::CCompositeProcessor::AssignOutPort(const wchar_t * port, IProcessor * inner_processor, const wchar_t * inner_port)
{
	if (_out_ports.find(port) != _out_ports.end())
		return false;

	_out_ports.insert(make_pair(port, Anchor(inner_processor, inner_port)));
	return true;
}
