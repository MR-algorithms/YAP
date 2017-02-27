#include "VariableManager.h"
#include "..\Implement\SharedObjectImpl.h"


using namespace Yap;

namespace Yap
{
	namespace _details
	{
		struct PropertyException
		{
			enum Type
			{
				PropertyNotFound,
				TypeNotMatch,
			};
			std::wstring property_name;
			Type type;
			PropertyException(const wchar_t * name, Type type_) : property_name(name), type(type_) {}
		};

		class PropertyImpl :
			public IProperty,
			public SharedObjectImpl
		{
		public:
			PropertyImpl(PropertyType type,
				const wchar_t * name,
				const wchar_t * description) :
				_type(type),
				_name(name),
				_description(description)
			{
			}

			virtual const wchar_t * GetName() override
			{
				return _name.c_str();
			}

			virtual PropertyType GetType() override
			{
				return _type;
			}

			virtual const wchar_t * GetDescription() override
			{
				return _description.c_str();
			}

		protected:
			std::wstring _name;
			std::wstring _description;
			PropertyType _type;
		};


		class DoubleProperty :
			public PropertyImpl,
			public IDouble,
			public IClonable
		{
		public:
			DoubleProperty(const wchar_t * name, const wchar_t * description, double value = 0.0)
				: PropertyImpl(PropertyFloat, name, description),
				_value(value) {}

			virtual double GetDouble()
			{
				return _value;
			}

			virtual void SetDouble(double value)
			{
				_value = value;
			}

			virtual IClonable * Clone() const override
			{
				return new(std::nothrow) DoubleProperty(_name.c_str(), _description.c_str(), _value);
			}

		protected:
			double _value;
		};

		class IntProperty :
			public PropertyImpl,
			public IInt,
			public IClonable
		{
		public:
			IntProperty(const wchar_t * name, const wchar_t * description, int value = 0)
				: PropertyImpl(PropertyInt, name, description), _value(value) {}

			virtual int GetInt() { return _value; }
			virtual void SetInt(int value) { _value = value; }
			virtual IClonable * Clone() const override
			{
				return new IntProperty(_name.c_str(), _description.c_str(), _value);
			}
		protected:
			int _value;
		};

		class BoolProperty :
			public PropertyImpl,
			public IBoolean,
			public IClonable
		{
		public:
			BoolProperty(const wchar_t * name, const wchar_t * description, bool value = 0)
				: PropertyImpl(PropertyBool, name, description), _value(value)	{}

			virtual bool GetBool()
			{
				return _value;
			}

			virtual void SetBool(bool value)
			{
				_value = value;
			}

			virtual IClonable * Clone() const override
			{
				return new (std::nothrow) BoolProperty(_name.c_str(), _description.c_str(), _value);
			}

		protected:
			bool _value;
		};

		class StringProperty :
			public PropertyImpl,
			public IString,
			public IClonable
		{
		public:
			StringProperty(const wchar_t * name,
				const wchar_t * description,
				const wchar_t * value = L"") :
				PropertyImpl(PropertyString, name, description),
				_value(value)
			{
			}

			virtual const wchar_t * GetString()
			{
				return _value.c_str();
			}

			virtual void SetString(const wchar_t * value)
			{
				_value = value;
			}

			virtual IClonable * Clone() const override
			{
				return new(std::nothrow) StringProperty(_name.c_str(), _description.c_str(), _value.c_str());
			}
		protected:
			std::wstring _value;
		};
	}  // end Yap::_details

VariableManager::VariableManager() :
	_properties(YapShared<ContainerImpl<IProperty>>(new ContainerImpl<IProperty>))
{
}

VariableManager::VariableManager(VariableManager * rhs) :
	_properties(YapShared<ContainerImpl<IProperty>>(rhs->_properties->Clone()))
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
		switch (type)
		{
		case PropertyBool:
			return _properties->Add(name, new BoolProperty(name, description));
		case PropertyInt:
			return _properties->Add(name, new IntProperty(name, description));
		case PropertyFloat:
			return _properties->Add(name, new DoubleProperty(name, description));
		case PropertyString:
			return _properties->Add(name, new StringProperty(name, description));
		default:
			return false;
		}
	}
	catch (std::bad_alloc&)
	{
		return false;
	}
}

SmartPtr<ContainerImpl<IProperty>> VariableManager::GetProperties()
{
	return _properties;
}

void VariableManager::SetInt(const wchar_t * name, int value)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties->Find(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyInt)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto int_value = dynamic_cast<IInt*>(property);
	assert(int_value != nullptr);
	int_value->SetInt(value);
}

int VariableManager::GetInt(const wchar_t * name)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties->Find(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyInt)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto int_value = dynamic_cast<IInt*>(property);
	assert(int_value != nullptr);

	return int_value->GetInt();
}

void VariableManager::SetFloat(const wchar_t * name, double value)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties->Find(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyFloat)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto float_value = dynamic_cast<IDouble*>(property);
	assert(float_value != nullptr);
	float_value->SetDouble(value);
}

double VariableManager::GetFloat(const wchar_t * name)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties->Find(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyFloat)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto float_value = dynamic_cast<IDouble*>(property);
	assert(float_value != nullptr);

	return float_value->GetDouble();
}

void VariableManager::SetBool(const wchar_t * name, bool value)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties->Find(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyBool)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto bool_value = dynamic_cast<IBoolean*>(property);
	assert(bool_value != nullptr);
	bool_value->SetBool(value);
}

bool VariableManager::GetBool(const wchar_t * name)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties->Find(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyBool)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto bool_value = dynamic_cast<IBoolean*>(property);
	assert(bool_value != nullptr);

	return bool_value->GetBool();
}
void VariableManager::SetString(const wchar_t * name, const wchar_t * value)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties->Find(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyString)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto string_value = dynamic_cast<IString*>(property);
	assert(string_value != nullptr);
	string_value->SetString(value);
}


const wchar_t * VariableManager::GetString(const wchar_t * name)
{
	assert(name != nullptr && name[0] != 0);

	auto property = _properties->Find(name);
	if (property == nullptr)
		throw PropertyException(name, PropertyException::PropertyNotFound);

	if (property->GetType() != PropertyString)
		throw PropertyException(name, PropertyException::TypeNotMatch);

	auto string_value = dynamic_cast<IString*>(property);
	assert(string_value != nullptr);
	return string_value->GetString();
}

}	// end Yap
