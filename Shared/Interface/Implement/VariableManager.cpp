#include "VariableManager.h"
#include "../Interfaces.h"
#include "ContainerImpl.h"
#include <fstream>
#include <string>

using namespace Yap;
using namespace std;

namespace Yap
{
	namespace _details
	{
		template <typename VALUE_TYPE>
		class ValueImpl : public IValue<VALUE_TYPE>
		{
		public:
			explicit ValueImpl(VALUE_TYPE value) : _value{ value } {}
			ValueImpl(const ValueImpl& rhs) : _value{ rhs.value } {}
			ValueImpl(const IValue<VALUE_TYPE>& rhs) : _value{ rhs.Get() } {}

			virtual VALUE_TYPE Get() const {
				return _value;
			}

			virtual void Set(const VALUE_TYPE value) {
				_value = value;
			}
		private:
			VALUE_TYPE _value;
		};

		class StringValueImpl : public IStringValue
		{
		public:
			StringValueImpl(const wchar_t * value) : _value{ value } {}
			StringValueImpl(const StringValueImpl& rhs) : _value{ rhs._value } {}
			StringValueImpl(const IStringValue& rhs) : _value{ rhs.Get() } {}

			virtual const wchar_t * Get() const override {
				return _value.c_str();
			}

			virtual void Set(const wchar_t * value) override {
				_value = value;
			}
		private:
			std::wstring _value;
		};


		class PropertyImpl :
			public IProperty
		{
			IMPLEMENT_SHARED(PropertyImpl)

		public:
			PropertyImpl(PropertyType type,
				const wchar_t * name,
				const wchar_t * description) :
				_type(type),
				_name(name),
				_description(description != nullptr ? description : L"")
			{
				switch (type)
				{
				case PropertyBool:
					_value_interface = new ValueImpl<bool>(false);
					break;
				case PropertyInt:
					_value_interface = new ValueImpl<int>(0);
					break;
				case PropertyFloat:
					_value_interface = new ValueImpl<double>(0.0);
					break;
				case PropertyString:
					_value_interface = new StringValueImpl(L"");
					break;
				default:
					throw PropertyException(name, PropertyException::InvalidType);
				}
			}

			PropertyImpl(const IProperty& rhs) :
				_type{ rhs.GetType() },
				_name{ rhs.GetName() },
				_description{ rhs.GetDescription() }
			{
				switch (rhs.GetType())
				{
				case PropertyBool:
					_value_interface = new ValueImpl<bool>(
						*reinterpret_cast<IBoolValue*>(const_cast<IProperty&>(rhs).ValueInterface()));
					break;
				case PropertyInt:
					_value_interface = new ValueImpl<int>(
						*reinterpret_cast<IIntValue*>(const_cast<IProperty&>(rhs).ValueInterface()));
					break;
				case PropertyFloat:
					_value_interface = new ValueImpl<double>(
						*reinterpret_cast<IDoubleValue*>(const_cast<IProperty&>(rhs).ValueInterface()));
					break;
				case PropertyString:
					_value_interface = new StringValueImpl(
						*reinterpret_cast<IStringValue*>(const_cast<IProperty&>(rhs).ValueInterface()));
					break;
				default:
					throw PropertyException(_name.c_str(), PropertyException::InvalidType);

				}
			}

			virtual PropertyType GetType() const override
			{
				return _type;
			}

			virtual const wchar_t * GetName() const override
			{
				return _name.c_str();
			}

			virtual const wchar_t * GetDescription() const override
			{
				return _description.c_str();
			}

			virtual void * ValueInterface() override
			{
				return _value_interface;
			}

		protected:
			std::wstring _name;
			std::wstring _description;
			PropertyType _type;
			
			void * _value_interface;
		};

	}  // end Yap::_details

VariableManager::VariableManager() :
	_properties(YapShared(new ContainerImpl<IProperty>))
{
}

VariableManager::VariableManager(IPropertyContainer * properties) :
	_properties(YapShared(properties))
{
}

VariableManager::VariableManager(const VariableManager& rhs) :
	_properties(YapShared(rhs.GetProperties()->Clone()))
{
}

VariableManager::~VariableManager()
{
}

using namespace _details;

bool VariableManager::AddProperty(PropertyType type,
	const wchar_t * name,
	const wchar_t * description)
{
	try
	{
		_properties->Add(name, new PropertyImpl(type, name, description));
		return true;
	}
	catch (std::bad_alloc&)
	{
		return false;
	}
}

IPropertyContainer* VariableManager::GetProperties()
{
	return _properties.get();
}

const IPropertyContainer* VariableManager::GetProperties() const
{
	return _properties.get();
}

IProperty * VariableManager::GetProperty(const wchar_t * name, PropertyType expected_type)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties->Find(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != expected_type)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	return property;
}

void VariableManager::SetInt(const wchar_t * name, int value)
{
	auto property = GetProperty(name, PropertyInt);
	assert(property->ValueInterface() != nullptr);
	reinterpret_cast<IIntValue*>(property->ValueInterface())->Set(value);
}

int VariableManager::GetInt(const wchar_t * name)
{
	auto property = GetProperty(name, PropertyInt);
	assert(property->ValueInterface() != nullptr);

	return reinterpret_cast<IIntValue*>(property->ValueInterface())->Get();
}

void VariableManager::SetFloat(const wchar_t * name, double value)
{
	auto property = GetProperty(name, PropertyFloat);
	assert(property->ValueInterface() != nullptr);

	reinterpret_cast<IDoubleValue*>(property->ValueInterface())->Set(value);
}

double VariableManager::GetFloat(const wchar_t * name)
{
	auto property = GetProperty(name, PropertyFloat);
	assert(property->ValueInterface() != nullptr);

	return reinterpret_cast<IDoubleValue*>(property->ValueInterface())->Get();
}

void VariableManager::SetBool(const wchar_t * name, bool value)
{
	auto property = GetProperty(name, PropertyBool);
	assert(property->ValueInterface() != nullptr);

	reinterpret_cast<IBoolValue*>(property->ValueInterface())->Set(value);
}

bool VariableManager::GetBool(const wchar_t * name)
{
	auto property = GetProperty(name, PropertyBool);
	assert(property->ValueInterface() != nullptr);

	return reinterpret_cast<IBoolValue*>(property->ValueInterface())->Get();
}

void VariableManager::SetString(const wchar_t * name, const wchar_t * value)
{
	auto property = GetProperty(name, PropertyString);
	assert(property->ValueInterface() != nullptr);

	reinterpret_cast<IStringValue*>(property->ValueInterface())->Set(value);
}


const wchar_t * VariableManager::GetString(const wchar_t * name)
{
	auto property = GetProperty(name, PropertyString);
	assert(property->ValueInterface() != nullptr);

	return reinterpret_cast<IStringValue*>(property->ValueInterface())->Get();
}

bool VariableManager::Load(const wchar_t *path)
{
	wifstream input(path);
	if (!input)
		return false;

	wstring line;
	while (getline(input, line, L'\n'))
	{
		ProcessLine(line);
	}

	return true;
}

bool VariableManager::ProcessLine(std::wstring& line)
{
	size_t pos = line.find_first_not_of(L" \t\n", 0);
	if (pos == wstring::npos)
		return true; // empty line

	if (line.substr(pos, 2) == L"//")
		return true;

	size_t separator_pos = line.find_first_of(L" \t", pos);
	if (separator_pos == wstring::npos)
		return false; // variable name should be on the same line as type. E.g. float D1;

	wstring type = line.substr(pos, separator_pos - pos);
	pos = line.find_first_not_of(L" \t", separator_pos);
	if (pos == wstring::npos)
		return false;  // variable name should be on the same line as type. E.g. float D1;

	separator_pos = line.find_first_of(L";", pos);
	if (separator_pos == wstring::npos)
		return false; // variable name should be on the same line as type. E.g. float D1;

	wstring id = line.substr(pos, separator_pos - pos);

	if (id.empty())
		return false;

	if (type == L"float")
	{
		return AddProperty(PropertyFloat, id.c_str(), L"");
	}
	else if (type == L"int")
	{
		return AddProperty(PropertyInt, id.c_str(), L"");
	}
	else if (type == L"string")
	{
		return AddProperty(PropertyString, id.c_str(), L"");
	}
	else if (type == L"bool")
	{
		return AddProperty(PropertyString, id.c_str(), L"");
	}
	else if (type.substr(0, 5) == L"array")
	{
		return true; // 暂未处理
	}
	else
	{
		return false;
	}

}

}	// end Yap
