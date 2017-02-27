#pragma once
#include "..\IProperty.h"
#include "..\IMemory.h"

namespace Yap {

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
	protected:
		SmartPtr<IPropertyContainer> _properties;
	};

}
