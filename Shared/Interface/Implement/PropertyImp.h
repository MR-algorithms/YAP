#pragma once

#ifndef PropertyImp_h__20160825
#define PropertyImp_h__20160825

#include <string>
#include <map>

#include "Interface/IProperties.h"

namespace Yap
{
	class CProperty : public IProperty
	{
	public:
		CProperty(PropertyType type, const wchar_t * name, const wchar_t * description);
		virtual const wchar_t * GetName() override;

		virtual PropertyType GetType() override;

		virtual const wchar_t * GetDescription() override;
	private:
		std::wstring _name;
		std::wstring _description;
		PropertyType _type;
	};

	class CFloatProperty : public CProperty, public IFloatValue
	{
	public:
		CFloatProperty(const wchar_t * name, const wchar_t * description, double value = 0.0)
			: CProperty(PropertyFloat, name, description), _value(value) {}
		virtual double GetValue() { return _value; }
		virtual void SetValue(double value) { _value = value; }

	protected:
		double _value;
	};

	class CIntProperty : public CProperty, public IIntValue
	{
	public:
		CIntProperty(const wchar_t * name, const wchar_t * description, int value = 0)
			: CProperty(PropertyInt, name, description), _value(value) {}
		virtual int GetValue() { return _value; }
		virtual void SetValue(int value) { _value = value; }

	protected:
		int _value;
	};

	class CBoolProperty : public CProperty, public IBoolValue
	{
	public:
		CBoolProperty(const wchar_t * name, const wchar_t * description, bool value = 0)
			: CProperty(PropertyBool, name, description), _value(value) {}
		virtual bool GetValue() { return _value; }
		virtual void SetValue(bool value) { _value = value; }

	protected:
		bool _value;
	};

	class CStringProperty : public CProperty, public IStringValue
	{
	public:
		CStringProperty(const wchar_t * name, const wchar_t * description, const wchar_t * value = L"")
			: CProperty(PropertyString, name, description), _value(value) {}
		virtual const wchar_t * GetValue() { return _value.c_str(); }
		virtual void SetValue(const wchar_t * value) { _value = value; }

	protected:
		std::wstring _value;
	};

	class CPropertyEnumeratorImp : public IPropertyEnumerator,
		public IDynamicObject
	{
	public:
		virtual IProperty * GetFirst() override;
		virtual IProperty * GetNext() override;
		virtual IProperty * GetProperty(const wchar_t * name) override;

		bool AddProperty(IProperty * property);

		bool GetBool(const wchar_t * id) const;
		//		double GetFloat(const wchar_t * id) const;
		//		const wchar_t * GetString(const wchar_t * id) const;
		//		const GetInt(const wchar_t * id) const;

		virtual void Delete() override;
	protected:
		std::map<std::wstring, IProperty*> _properties;
		std::map<std::wstring, IProperty*>::iterator _current;
	};
};

#endif // PropertyImp_h__