#pragma once
#include "..\interfaces.h"
#include "..\smartptr.h"
#include <string>
#include <map>

namespace Yap {

    struct PropertyException
    {
        enum Type
        {
            PropertyNotFound,
            TypeNotMatch,
            InvalidType,
			OutOfRange,
			InvalidPropertyId,
			NotStruct,
        };
        std::wstring property_name;
        Type type;
        PropertyException(const wchar_t * name, Type type_) : property_name(name), type(type_) {}
    };

    class VariableManager
    {
    public:
        VariableManager();
        explicit VariableManager(IPropertyContainer * properties);
        VariableManager(const VariableManager& rhs);

        ~VariableManager();

        bool AddProperty(int type, const wchar_t * name, const wchar_t * description);
        bool AddProperty(const wchar_t * type, const wchar_t * name, const wchar_t * description);
        bool AddProperty(IProperty* property);

        IPropertyContainer * GetProperties();
        const IPropertyContainer * GetProperties() const;

		template <typename T>
		T Get(const wchar_t * id) {
			std::wstring property_id{ id };

			auto left_square_pos = property_id.find_last_of(L'[');
			if (left_square_pos != wstring::npos) {
				return Element<T>(property_id);
			}
			else {
				auto property = GetProperty(id, property_type_id<T>::type);
				assert(property != nullptr && "If parameter not found, GetProperty() should throw an PropertyException.");
				assert(property->ValueInterface() != nullptr);
				return reinterpret_cast<IValue<T>*>(property->ValueInterface())->Get();
			}
		}

		template<typename T>
		void Set(const wchar_t * id, T value) {
			std::wstring property_id{ id };

			auto left_square_pos = property_id.find_last_of(L'[');
			if (left_square_pos != std::wstring::npos) {
				Element<T>(property_id) = value;
			}
			else {
				auto property = GetProperty(id, property_type_id<T>::type);
				assert(property != nullptr && "If parameter not found, GetProperty() should throw an PropertyException.");
				assert(property->ValueInterface() != nullptr);
				reinterpret_cast<IValue<T>*>(property->ValueInterface())->Set(value);
			}
		}
		
		bool ResizeArray(const wchar_t * id, size_t size);

		template<typename T>
		T* GetArray(const wchar_t * id) {
			auto array = GetProperty(id, property_type_id<T>::array_type);
			if (array == nullptr)
				return nullptr;

			assert(array->GetType() == property_type_id<T>::array_type);
			auto array_interface = reinterpret_cast<IArrayValue<T>*>(array->ValueInterface());
			assert(array_interface != nullptr);
			return array_interface->Elements();
		}

		static std::shared_ptr<VariableManager> Load(const wchar_t * path);

        void Reset();
        bool TypeExists(const wchar_t * type_id) const;
        bool PropertyExists(const wchar_t * property_id) const;
        const IProperty * GetType(const wchar_t * type_id) const;
        bool AddType(const wchar_t * type_id, IProperty *type);
        bool AddType(const wchar_t * type_id, IContainer<IProperty> * properties);

    protected:

		template <typename T>
		T & Element(const std::wstring& id)
		{
			auto left_square_pos = id.find_last_of(L'[');
			assert(left_square_pos != std::wstring::npos);

			auto right_square_pos = id.find_last_of(L']', left_square_pos);
			if (right_square_pos == std::wstring::npos)
				throw PropertyException(id.c_str(), PropertyException::PropertyNotFound);

			auto array_property = GetProperty(id.substr(0, left_square_pos).c_str(), property_type_id<T>::array_type);
			if (array_property == nullptr)
				throw PropertyException(id.c_str(), PropertyException::PropertyNotFound);

			auto index = _wtoi(id.substr(left_square_pos + 1, right_square_pos - left_square_pos - 1).c_str());
			auto array_interface = reinterpret_cast<IArrayValue<T>*>(array_property->ValueInterface());
			assert(array_interface != nullptr);

			if (index < 0 || index >= int(array_interface->GetSize()))
				throw PropertyException(id.c_str(), PropertyException::OutOfRange);

			return array_interface->Elements()[index];
		}

		bool InitTypes();
        IProperty * GetProperty(const wchar_t * name, int expected_type);
		IProperty * GetProperty(IPropertyContainer * properties, const wchar_t * name, int type);
        SmartPtr<IPropertyContainer> _properties;

        std::map<std::wstring, SmartPtr<IProperty>> _types;
    };

}
