#pragma once

#ifndef PropertyImp_h__20160825
#define PropertyImp_h__20160825

#include <string>
#include <map>

#include "Interface/IProperties.h"
#include "Interface/IMemory.h"

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


	class DoubleProperty : public CProperty, public IDouble
	{
	public:
		DoubleProperty(const wchar_t * name, const wchar_t * description, double value = 0.0);
		virtual double GetDouble();
		virtual void SetDouble(double value);

	protected:
		double _value;
	};

	class IntProperty : public CProperty, public IInt
	{
	public:
		IntProperty(const wchar_t * name, const wchar_t * description, int value = 0);
		virtual int GetInt();
		virtual void SetInt(int value);

	protected:
		int _value;
	};

	class BoolProperty : public CProperty, public IBoolean
	{
	public:
		BoolProperty(const wchar_t * name, const wchar_t * description, bool value = 0);
		virtual bool GetBool();
		virtual void SetBool(bool value);

	protected:
		bool _value;
	};

	class StringProperty : public CProperty, public IString
	{
	public:
		StringProperty(const wchar_t * name, const wchar_t * description, const wchar_t * value = L"");
		virtual const wchar_t * GetString();
		virtual void SetString(const wchar_t * value);

	protected:
		std::wstring _value;
	};


	class PropertyContainerImpl : public IPropertyContainer,
		public IDynamicObject
	{
		class PropertyIterImpl : public IIterator<IProperty>, public IDynamicObject
		{
		public:
			explicit PropertyIterImpl(PropertyContainerImpl& container);	

			virtual IProperty * GetFirst() override;
			virtual IProperty * GetNext() override;
			virtual void DeleteThis() override;
		private:
			std::map<std::wstring, IProperty*>::iterator _current;
			PropertyContainerImpl& _container;

			friend class PropertyContainerImpl;
		};

	public:
		virtual IProperty * GetProperty(const wchar_t * name) override;

		bool AddProperty(IProperty * property);

		bool GetBool(const wchar_t * id) const;

		//		double GetFloat(const wchar_t * id) const;
		//		const wchar_t * GetString(const wchar_t * id) const;
		//		const GetInt(const wchar_t * id) const;

		virtual void DeleteThis() override;
	protected:
		std::map<std::wstring, IProperty*> _properties;
		std::map<std::wstring, IProperty*>::iterator _current;

		// Inherited via IPropertyContainer
		virtual IPropertyIter * GetIterator() override;


		friend class PropertyIterImpl;
	};
};

#endif // PropertyImp_h__