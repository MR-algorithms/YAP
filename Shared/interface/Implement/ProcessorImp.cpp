#include "ProcessorImp.h"

#include "Interface/YapInterfaces.h"

#include <iostream>
#include <algorithm>
#include <assert.h>

using namespace std;
using namespace Yap;

CProcessorImp::CProcessorImp(const wchar_t * class_id) :
	_system_variables(nullptr),
	_class_id(class_id)
{
}

Yap::CProcessorImp::CProcessorImp(const CProcessorImp& rhs) :
	_input_ports(rhs._input_ports),
	_output_ports(rhs._output_ports),
	_properties(rhs._properties),
	_instance_id(rhs._instance_id),
	_class_id(rhs._class_id),
	_system_variables(nullptr)
{
	_links.clear();
	_property_links.clear();
}

void Yap::CProcessorImp::Release()
{
	delete this;
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
	return OnInit();
}

bool CProcessorImp::Link(const wchar_t * out_port, IProcessor* next_processor, const wchar_t * in_port)
{
	assert(wcslen(out_port) != 0);
	assert(next_processor != nullptr);
	assert(wcslen(in_port) != 0);

	if (!CanLink(out_port, next_processor, in_port))
		return false;

	_links.insert(std::make_pair(out_port, Anchor(next_processor, in_port)));
	return true;
}

bool CProcessorImp::AddInputPort(const wchar_t * name, unsigned int dimensions, int data_type)
{
	return _input_ports.AddPort(name, dimensions, data_type);
}

bool CProcessorImp::AddOutputPort(const wchar_t * name, unsigned int dimensions, int data_type)
{
	return _output_ports.AddPort(name, dimensions, data_type);
}

bool CProcessorImp::Feed(const wchar_t * out_port, IData * data)
{
	assert(wcslen(out_port) != 0);
	assert(data != nullptr);

	auto range = _links.equal_range(out_port);
	for (auto iter = range.first; iter != range.second; ++iter)
	{
		auto link = iter->second;
		assert(link.processor != nullptr);
		if (link.processor == nullptr || !link.processor->Input(link.port.c_str(), data))
			return false;
	}

	return true;
}

bool CProcessorImp::AddProperty(PropertyType type,
	const wchar_t * name,
	const wchar_t * description)
{
	try
	{
		switch (type)
		{
		case PropertyBool:
			return _properties.AddProperty(new CBoolProperty(name, description));
		case PropertyInt:
			return _properties.AddProperty(new CIntProperty(name, description));
		case PropertyFloat:
			return _properties.AddProperty(new CFloatProperty(name, description));
		case PropertyString:
			return _properties.AddProperty(new CStringProperty(name, description));
		default:
			return false;
		}
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

void CProcessorImp::SetInt(const wchar_t * name, int value)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties.GetProperty(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyInt)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto int_value = dynamic_cast<IIntValue*>(property);
	assert(int_value != nullptr);
	int_value->SetValue(value);
}

int CProcessorImp::GetInt(const wchar_t * name)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties.GetProperty(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyInt)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto int_value = dynamic_cast<IIntValue*>(property);
	assert(int_value != nullptr);

	return int_value->GetValue();
}

void CProcessorImp::SetFloat(const wchar_t * name, double value)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties.GetProperty(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyFloat)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto float_value = dynamic_cast<IFloatValue*>(property);
	assert(float_value != nullptr);
	float_value->SetValue(value);
}

double CProcessorImp::GetFloat(const wchar_t * name)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties.GetProperty(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyFloat)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto float_value = dynamic_cast<IFloatValue*>(property);
	assert(float_value != nullptr);

	return float_value->GetValue();
}

void CProcessorImp::SetBool(const wchar_t * name, bool value)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties.GetProperty(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyBool)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto bool_value = dynamic_cast<IBoolValue*>(property);
	assert(bool_value != nullptr);
	bool_value->SetValue(value);
}


void CProcessorImp::SetString(const wchar_t * name, const wchar_t * value)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties.GetProperty(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyString)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto string_value = dynamic_cast<IStringValue*>(property);
	assert(string_value != nullptr);
	string_value->SetValue(value);
}


const wchar_t * CProcessorImp::GetString(const wchar_t * name)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties.GetProperty(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyString)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto string_value = dynamic_cast<IStringValue*>(property);
	assert(string_value != nullptr);
	return string_value->GetValue();
}

const wchar_t * Yap::CProcessorImp::GetClassId()
{
	return _class_id.c_str();
}

void Yap::CProcessorImp::SetClassId(const wchar_t * id)
{
	_class_id = id;
}

const wchar_t * Yap::CProcessorImp::GetInstanceId()
{
	return _instance_id.c_str();
}

void Yap::CProcessorImp::SetInstanceId(const wchar_t * instance_id)
{
	_instance_id = instance_id;
}

bool Yap::CProcessorImp::LinkProperty(const wchar_t * property_id, const wchar_t * param_id)
{
	if (_properties.GetProperty(property_id) != nullptr)
	{
		return false;
	}

	_property_links.insert(make_pair(wstring(property_id), wstring(param_id)));

	return true;
}

bool Yap::CProcessorImp::UpdateProperties(IPropertyEnumerator * params)
{
	for (auto link : _property_links)
	{
		auto property = _properties.GetProperty(link.first.c_str());
		if (property == nullptr)
			return false;

		auto system_variable = params->GetProperty(link.second.c_str());
		if (system_variable == nullptr)
			return false;

		if (property->GetType() != system_variable->GetType())
			return false;

		try
		{
			switch (property->GetType())
			{
			case PropertyBool:
				dynamic_cast<IBoolValue&>(*property).SetValue(dynamic_cast<IBoolValue&>(*system_variable).GetValue());
				break;
			case PropertyInt:
				dynamic_cast<IIntValue&>(*property).SetValue(dynamic_cast<IIntValue&>(*system_variable).GetValue());
				break;
			case PropertyFloat:
				dynamic_cast<IFloatValue&>(*property).SetValue(dynamic_cast<IFloatValue&>(*system_variable).GetValue());
				break;
			case PropertyString:
				dynamic_cast<IStringValue&>(*property).SetValue(dynamic_cast<IStringValue&>(*system_variable).GetValue());
				break;
			default:
				return false;
			}
		}
		catch (std::bad_cast&)
		{
			return false;
		}
	}

	return true;
}

bool Yap::CProcessorImp::CanLink(const wchar_t * source_output_name, 
	IProcessor * next, 
	const wchar_t * next_input_name)
{
	assert(next != nullptr);
	assert(wcslen(source_output_name) != 0);
	assert(wcslen(next_input_name) != 0);

	auto out_port = _output_ports.GetPort(source_output_name);
	if (out_port == nullptr)
		return false;

	auto in_port_enumerator = next->GetInputPortEnumerator();
	if (in_port_enumerator == nullptr)
		return false;

	auto in_port = in_port_enumerator->GetPort(next_input_name);
	if (in_port == nullptr)
		return false;

	if ((out_port->GetDataType() & in_port->GetDataType()) == 0)
		return false;

	TODO(Refine the processing of YAP_ANY_DIMENSION and support of multiple data type.);

	return (out_port->GetDimensions() == in_port->GetDimensions() ||
		out_port->GetDimensions() == YAP_ANY_DIMENSION ||
		in_port->GetDimensions() == YAP_ANY_DIMENSION);
}

bool Yap::CProcessorImp::OutportLinked(const wchar_t * out_port_name) const
{
	return (_links.find(out_port_name) != _links.end());
}

bool CPortEnumerator::AddPort(const wchar_t * name,
	unsigned int dimensions, 
	int data_type)
{
	try
	{
		shared_ptr<IPort> port(new CPort(name, dimensions, data_type));
		_ports.insert(std::make_pair(port->GetName(), port));
	}
	catch (bad_alloc&)
	{
		return false;
	}

	return true;
}

IPort * CPortEnumerator::GetFirstPort()
{
	_current_port = _ports.begin();
	return (_current_port != _ports.end()) ? _current_port->second.get() : nullptr;
}

IPort * CPortEnumerator::GetNextPort()
{
	return (_current_port == _ports.end() || ++_current_port == _ports.end()) ? 
		nullptr : _current_port->second.get();
}

Yap::IPort * Yap::CPortEnumerator::GetPort(const wchar_t * name)
{
	auto iter = _ports.find(name);
	return (iter != _ports.end()) ? iter->second.get() : nullptr;
}

CPort::CPort(const wchar_t * name, unsigned int dimensions, int data_type) : 
	_name(name), 
	_dimensions(dimensions), 
	_data_type(data_type)
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

int CPort::GetDataType()
{
	return _data_type;
}

IProperty * CPropertyEnumeratorImp::GetFirst()
{
	if (_properties.empty())
		return nullptr;

	_current = _properties.begin();

	return _current->second;
}

IProperty * CPropertyEnumeratorImp::GetNext()
{
	return (_current != _properties.end() && ++_current != _properties.end()) ? 
		_current->second : nullptr;
}

IProperty * CPropertyEnumeratorImp::GetProperty(const wchar_t * name)
{
	auto iter = _properties.find(name);
	return (iter != _properties.end()) ? iter->second : nullptr;
}

bool Yap::CPropertyEnumeratorImp::GetBool(const wchar_t * id) const
{
	assert(id != nullptr && id[0] != 0);

	auto iter = _properties.find(id);
	if (iter == _properties.end())
		throw PropertyException(id, PropertyException::PropertyNotFound);

	auto property = iter->second;
	if (property->GetType() != PropertyBool)
		throw PropertyException(id, PropertyException::TypeNotMatch);

	auto bool_value = dynamic_cast<IBoolValue*>(property);
	assert(bool_value != nullptr);

	return bool_value->GetValue();
}

bool CProcessorImp::GetBool(const wchar_t * name)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties.GetProperty(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyBool)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto bool_value = dynamic_cast<IBoolValue*>(property);
	assert(bool_value != nullptr);

	return bool_value->GetValue();
}

Yap::CProperty::CProperty(PropertyType type, const wchar_t * name, const wchar_t * description) :
	_type(type),
	_name(name),
	_description(description)
{
}

const wchar_t * Yap::CProperty::GetName()
{
	return _name.c_str();
}

Yap::PropertyType Yap::CProperty::GetType()
{
	return _type;
}

const wchar_t * Yap::CProperty::GetDescription()
{
	return _description.c_str();
}
