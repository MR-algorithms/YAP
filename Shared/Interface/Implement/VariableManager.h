#pragma once
#include "..\interfaces.h"
#include "ContainerImpl.h"
#include "..\smartptr.h"
#include <string>
#include <vector>
#include <map>

namespace Yap {

	struct PropertyException
	{
		enum Type
		{
			PropertyNotFound,
			TypeNotMatch,
			InvalidType,
		};
		std::wstring property_name;
		Type type;
		PropertyException(const wchar_t * name, Type type_) : property_name(name), type(type_) {}
	};

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

		template <typename VALUE_TYPE>
		class ArrayValueImpl : public IArrayValue<VALUE_TYPE>
		{
		public:
			ArrayValueImpl() {}

			ArrayValueImpl(size_t size)
			{
				_elements.resize(size);
			}

			virtual size_t GetSize() const override
			{
				return _elements.size();
			}

			virtual void SetSize(size_t size) override
			{
				_elements.resize(size);
			}

			virtual VALUE_TYPE * Elements() override
			{
				return _elements.data();
			}
		private:
			std::vector<VALUE_TYPE> _elements;
		};

		template<>
		class ArrayValueImpl<bool> : public IArrayValue<bool>
		{
		public:
			ArrayValueImpl()
			{}

			ArrayValueImpl(size_t size)
			{
				_elements.resize(size);
			}

			virtual size_t GetSize() const override
			{
				return _elements.size();
			}

			virtual void SetSize(size_t size) override
			{
				_elements.resize(size);
			}

			virtual bool * Elements() override
			{
				return reinterpret_cast<bool*>(_elements.data());
			}

		private:
			std::vector<unsigned char> _elements;
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
				case PropertyBoolArray:
					_value_interface = new ArrayValueImpl<bool>();
					break;
				case PropertyIntArray:
					_value_interface = new ArrayValueImpl<int>();
					break;
				case PropertyFloatArray:
					_value_interface = new ArrayValueImpl<double>();
					break;
				case PropertyStringArray:
					_value_interface = new ArrayValueImpl<std::wstring>();
					break;
				case PropertyStruct:
					_value_interface = nullptr;
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

			virtual void SetName(const wchar_t * name) override
			{
				_name = name;
			}

			virtual const wchar_t * GetDescription() const override
			{
				return _description.c_str();
			}

			virtual void SetDescription(const wchar_t * description) override
			{
				_description = description;
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

	class VariableManager
	{
	public:
		VariableManager();
		explicit VariableManager(IPropertyContainer * properties);
		VariableManager(const VariableManager& rhs);

		~VariableManager();

		bool AddProperty(PropertyType type, const wchar_t * name, const wchar_t * description);
        bool AddProperty(const wchar_t * type, const wchar_t * name, const wchar_t * description);
        bool AddProperty(IProperty* property);

		IPropertyContainer * GetProperties();
		const IPropertyContainer * GetProperties() const;

		void SetInt(const wchar_t * name, int value);
		int GetInt(const wchar_t * name);
		void SetFloat(const wchar_t * name, double value);
		double GetFloat(const wchar_t * name);
		void SetBool(const wchar_t * name, bool value);
		bool GetBool(const wchar_t * name);
		void SetString(const wchar_t * name, const wchar_t * value);
		const wchar_t * GetString(const wchar_t * name);

        static std::shared_ptr<VariableManager> Load(const wchar_t * path);

		void Reset();
        bool TypeExists(const wchar_t * type_id) const;
        bool PropertyExists(const wchar_t * property_id) const;
        IProperty * GetType(const wchar_t * type_id) const;
        bool AddType(const wchar_t * type_id, IProperty *type);
	protected:
        bool InitTypes();
        IProperty * GetProperty(const wchar_t * name, PropertyType expected_type);
		SmartPtr<IPropertyContainer> _properties;

        std::map<std::wstring, std::shared_ptr<IProperty>> _types;
	};

}
