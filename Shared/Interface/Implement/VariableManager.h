#pragma once
#include "..\interfaces.h"
#include "..\smartptr.h"
#include <string>

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

        bool Load(const wchar_t * path);
		void Reset();

	protected:
		IProperty * GetProperty(const wchar_t * name, PropertyType expected_type);
		SmartPtr<IPropertyContainer> _properties;
        bool ProcessLine(std::wstring& line);
	};

}
