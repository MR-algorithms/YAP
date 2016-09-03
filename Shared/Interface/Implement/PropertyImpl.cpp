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

IClonable * Yap::StringProperty::Clone()
{
	return new(std::nothrow) StringProperty(_name.c_str(), _description.c_str(), _value.c_str());
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

IClonable * Yap::BoolProperty::Clone()
{
	return new (std::nothrow) BoolProperty(_name.c_str(), _description.c_str(), _value);
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

IClonable * Yap::DoubleProperty::Clone() 
{
	return new(std::nothrow) DoubleProperty(_name.c_str(), _description.c_str(), _value);
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

IClonable * Yap::IntProperty::Clone()
{
	return new IntProperty(_name.c_str(), _description.c_str(), _value);
}
