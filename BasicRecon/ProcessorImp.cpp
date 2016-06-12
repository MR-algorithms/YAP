#include "stdafx.h"
#include "ProcessorImp.h"
#include <iostream>
#include <algorithm>
#include "..\Interface\Interface.h"

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

bool CProcessorImp::AddProperty(const wchar_t * name, PropertyType type)
{
	try
	{
		return _properties.AddProperty(new CPropertyImp(name, type));
	}
	catch (std::bad_alloc&)
	{
		return false;
	}
}

IPropertyEnumerator * CProcessorImp::GetProperties()
{
	return &_properties;
}

void CProcessorImp::SetIntProperty(const wchar_t * name, int value)
{
	(reinterpret_cast<CIntValue*>(_properties.GetProperty(name)->GetValueInterface()))->SetValue(value);
}

int CProcessorImp::GetIntProperty(const wchar_t * name)
{
	return (reinterpret_cast<CIntValue*>(_properties.GetProperty(name)->GetValueInterface()))->GetValue();
}

void CProcessorImp::SetFloatProperty(const wchar_t * name, double value)
{
	(reinterpret_cast<CFloatValue*>(_properties.GetProperty(name)->GetValueInterface()))->SetValue(value);
}

double CProcessorImp::GetFloatProperty(const wchar_t * name)
{
	return (reinterpret_cast<CFloatValue*>(_properties.GetProperty(name)->GetValueInterface()))->GetValue();
}

void CProcessorImp::SetBoolProperty(const wchar_t * name, bool value)
{
	(reinterpret_cast<CBoolValue*>(_properties.GetProperty(name)->GetValueInterface()))->SetValue(value);
}

bool CProcessorImp::GetBoolProperty(const wchar_t * name)
{
	return (reinterpret_cast<CBoolValue*>(_properties.GetProperty(name)->GetValueInterface()))->GetValue();
}

void CProcessorImp::SetStringProperty(const wchar_t * name, const wchar_t * value)
{
	(reinterpret_cast<CStringValue*>(_properties.GetProperty(name)->GetValueInterface()))->SetValue(value);
}

const wchar_t * CProcessorImp::GetStringProperty(const wchar_t * name)
{
	return (reinterpret_cast<CStringValue*>(_properties.GetProperty(name)->GetValueInterface()))->GetValue();
}

bool CPortEnumerator::AddPort(IPort* port)
{
	if (port == nullptr)
		return false;

	shared_ptr<IPort> p(port);
	_ports.insert(std::make_pair(port->GetName(), p));

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

IProperty * CPropertyEnumerator::GetFirst()
{
	if (_properties.empty())
		return nullptr;

	_current = _properties.begin();

	return _current->second;
}

IProperty * CPropertyEnumerator::GetNext()
{
	return (_current != _properties.end() && ++_current != _properties.end()) ? 
		_current->second : nullptr;
}

IProperty * CPropertyEnumerator::GetProperty(const wchar_t * name)
{
	auto iter = _properties.find(name);
	return (iter != _properties.end()) ? iter->second : nullptr;
}
