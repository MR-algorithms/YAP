#include "PropertyImpl.h"

using namespace Yap;

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

Yap::StringProperty::StringProperty(const wchar_t * name, 
									const wchar_t * description, 
									const wchar_t * value): 
	CProperty(PropertyString, name, description), 
	_value(value) 
{
}

const wchar_t * Yap::StringProperty::GetString()
{
	return _value.c_str(); 
}

void Yap::StringProperty::SetString(const wchar_t * value)
{
	_value = value; 
}

Yap::BoolProperty::BoolProperty(const wchar_t * name, const wchar_t * description, bool value)
	: CProperty(PropertyBool, name, description), _value(value) {}

inline bool Yap::BoolProperty::GetBool() 
{
	return _value;
}

void Yap::BoolProperty::SetBool(bool value) 
{
	_value = value;
}

Yap::DoubleProperty::DoubleProperty(const wchar_t * name, 
									const wchar_t * description, 
									double value):
	CProperty(PropertyFloat, name, description), 
	_value(value)
{
}

double Yap::DoubleProperty::GetDouble() 
{
	return _value;
}

void Yap::DoubleProperty::SetDouble(double value) 
{
	_value = value; 
}

Yap::IntProperty::IntProperty(const wchar_t * name, const wchar_t * description, int value)
	: CProperty(PropertyInt, name, description), _value(value) 
{
}

int Yap::IntProperty::GetInt() 
{
	return _value;
}

void Yap::IntProperty::SetInt(int value)
{ 
	_value = value; 
}

PropertyContainerImpl::PropertyIterImpl::PropertyIterImpl(PropertyContainerImpl & container) :
	_container(container),
	_current(container._properties.begin())
{
}

IProperty * PropertyContainerImpl::PropertyIterImpl::GetFirst()
{
	if (_container._properties.empty())
		return nullptr;

	_current = _container._properties.begin();

	return _current->second;
}

IProperty * PropertyContainerImpl::PropertyIterImpl::GetNext()
{
	if (_container._properties.empty() || _current == _container._properties.end() ||
		++_current == _container._properties.end())
	{
		return nullptr;
	}
	else
	{
		return _current->second;
	}
}

void PropertyContainerImpl::PropertyIterImpl::DeleteThis()
{
	delete this;
}

IProperty * PropertyContainerImpl::Find(const wchar_t * name)
	{
		auto iter = _properties.find(name);
		return (iter != _properties.end()) ? iter->second : nullptr;
	}

	bool Yap::PropertyContainerImpl::AddProperty(IProperty * property)
	{
		if (property == nullptr)
			return false;

		if (_properties.find(property->GetName()) != _properties.end())
			return false;

		_properties.insert(std::make_pair(property->GetName(), property));
		return true;
	}

bool Yap::PropertyContainerImpl::GetBool(const wchar_t * id) const
{
	assert(id != nullptr && id[0] != 0);

	auto iter = _properties.find(id);
	if (iter == _properties.end())
		throw PropertyException(id, PropertyException::PropertyNotFound);

	auto property = iter->second;
	if (property->GetType() != PropertyBool)
		throw PropertyException(id, PropertyException::TypeNotMatch);

	auto bool_value = dynamic_cast<IBoolean*>(property);
	assert(bool_value != nullptr);

	return bool_value->GetBool();
}

void Yap::PropertyContainerImpl::DeleteThis()
{
	delete this;
}

IPropertyIter * Yap::PropertyContainerImpl::GetIterator()
{
	try
	{
		return new PropertyIterImpl(*this);
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}
