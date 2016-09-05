#include "PropertyImpl.h"

using namespace Yap;

Yap::PropertyImpl::PropertyImpl(PropertyType type, 
						  const wchar_t * name, 
						  const wchar_t * description) :
	_type(type),
	_name(name),
	_description(description)
{
}

const wchar_t * Yap::PropertyImpl::GetName()
{
	return _name.c_str();
}

Yap::PropertyType Yap::PropertyImpl::GetType()
{
	return _type;
}

const wchar_t * Yap::PropertyImpl::GetDescription()
{
	return _description.c_str();
}

Yap::StringProperty::StringProperty(const wchar_t * name, 
									const wchar_t * description, 
									const wchar_t * value): 
	PropertyImpl(PropertyString, name, description), 
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

IClonable * Yap::StringProperty::Clone() const 
{
	return new(std::nothrow) StringProperty(_name.c_str(), _description.c_str(), _value.c_str());
}

Yap::BoolProperty::BoolProperty(const wchar_t * name, const wchar_t * description, bool value)
	: PropertyImpl(PropertyBool, name, description), _value(value)
{
}

inline bool Yap::BoolProperty::GetBool() 
{
	return _value;
}

void Yap::BoolProperty::SetBool(bool value) 
{
	_value = value;
}

IClonable * Yap::BoolProperty::Clone() const
{
	return new (std::nothrow) BoolProperty(_name.c_str(), _description.c_str(), _value);
}

Yap::DoubleProperty::DoubleProperty(const wchar_t * name, 
									const wchar_t * description, 
									double value):
	PropertyImpl(PropertyFloat, name, description), 
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

IClonable * Yap::DoubleProperty::Clone() const
{
	return new(std::nothrow) DoubleProperty(_name.c_str(), _description.c_str(), _value);
}

Yap::IntProperty::IntProperty(const wchar_t * name, const wchar_t * description, int value)
	: PropertyImpl(PropertyInt, name, description), _value(value) 
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

IClonable * Yap::IntProperty::Clone() const
{
	return new IntProperty(_name.c_str(), _description.c_str(), _value);
}
