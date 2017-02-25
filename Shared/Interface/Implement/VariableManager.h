#pragma once
#include "ContainerImpl.h"
#include "..\IProperty.h"

namespace Yap {

	class VariableManager
	{
	public:
		VariableManager();
		VariableManager(VariableManager * rhs);

		~VariableManager();

		bool AddProperty(PropertyType type, const wchar_t * name, const wchar_t * description);
		SmartPtr<ContainerImpl<IProperty>> GetProperties();

		void SetInt(const wchar_t * name, int value);
		int GetInt(const wchar_t * name);
		void SetFloat(const wchar_t * name, double value);
		double GetFloat(const wchar_t * name);
		void SetBool(const wchar_t * name, bool value);
		bool GetBool(const wchar_t * name);
		void SetString(const wchar_t * name, const wchar_t * value);
		const wchar_t * GetString(const wchar_t * name);
	protected:
		SmartPtr<ContainerImpl<IProperty>> _properties;
	};

}
