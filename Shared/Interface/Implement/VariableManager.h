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
        const IProperty * GetType(const wchar_t * type_id) const;
        bool AddType(const wchar_t * type_id, IProperty *type);
        bool AddType(const wchar_t * type_id, IContainer<IProperty> * properties);

    protected:
        bool InitTypes();
        IProperty * GetProperty(const wchar_t * name, PropertyType expected_type);
        SmartPtr<IPropertyContainer> _properties;

        std::map<std::wstring, SmartPtr<IProperty>> _types;
    };

}
