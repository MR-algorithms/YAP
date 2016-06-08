#include "stdafx.h"
#include "ProcessorImp.h"
#include <iostream>
#include <algorithm>

using namespace std;

CProcessorImp::CProcessorImp()
{
}


CProcessorImp::~CProcessorImp()
{
}

IPortEnumerator * CProcessorImp::GetInputPortEnumerator()
{
	return &_input_ports;
}

IPortEnumerator * CProcessorImp::GetOutputPortEnumerator()
{
	return &_output_ports;
}

bool CProcessorImp::Init()
{
	return true;
}

bool CProcessorImp::Input(const wchar_t * port, IData * data)
{

	return true;
}

bool CProcessorImp::Link(const wchar_t * out_port, IProcessor* next_processor, wchar_t * in_port)
{
	_links.insert(std::make_pair(out_port, Anchor(next_processor, in_port)));
	return true;
}

bool CProcessorImp::AddInputPort(const wchar_t * name, unsigned int dimensions, DataType data_type)
{
	try
	{
		return _input_ports.AddPort(new CPort(name, dimensions, data_type));
	}
	catch (std::bad_alloc&)
	{
		return false;
	}
}

bool CProcessorImp::AddOutputPort(const wchar_t * name, unsigned int dimensions, DataType data_type)
{
	try
	{
		return _output_ports.AddPort(new CPort(name, dimensions, data_type));
	}
	catch (std::bad_alloc&)
	{
		return false;
	}
}

void CProcessorImp::Feed(const wchar_t * out_port, IData * data)
{
	auto range = _links.equal_range(out_port);
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		iter->second.processor->Input(iter->second.in_port.c_str(), data);
	}
}

bool CPortEnumerator::AddPort(IPort* port)
{
	if (port == nullptr)
		return false;
	_ports.insert(std::make_pair(port->GetName(), port));

	return true;
}

IPort * CPortEnumerator::GetFirstPort()
{
	_current_port = _ports.begin();
	if (_current_port != _ports.end())
	{
		return _current_port->second.get();
	}

	return nullptr;
}

IPort * CPortEnumerator::GetNextPort()
{
	return (_current_port == _ports.end() || ++_current_port == _ports.end()) ? 
		nullptr : _current_port->second.get();
}

CPort::CPort(const wchar_t * name, unsigned int dimensions, DataType data_type) : _name(name), _dimensions(dimensions), _data_type(data_type)
{
}

const wchar_t * CPort::GetName()
{
	return _name.c_str();
}

unsigned int CPort::GetDimensions()
{
	return _dimensions;
}

DataType CPort::GetDataType()
{
	return _data_type;
}
