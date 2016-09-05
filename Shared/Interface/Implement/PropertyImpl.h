#pragma once

#ifndef PropertyImp_h__20160825
#define PropertyImp_h__20160825

#include <string>
#include <map>

#include "Interface/IProperty.h"
#include "Interface/Implement/SharedObjectImpl.h"

namespace Yap
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
		PropertyImpl(PropertyType type, const wchar_t * name, const wchar_t * description);
		virtual const wchar_t * GetName() override;

		virtual PropertyType GetType() override;

		virtual const wchar_t * GetDescription() override;

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
		DoubleProperty(const wchar_t * name, const wchar_t * description, double value = 0.0);
		virtual double GetDouble();
		virtual void SetDouble(double value);

		virtual IClonable * Clone() const override;
	protected:
		double _value;
	};

	class IntProperty : 
		public PropertyImpl, 
		public IInt,
		public IClonable
	{
	public:
		IntProperty(const wchar_t * name, const wchar_t * description, int value = 0);
		virtual int GetInt();
		virtual void SetInt(int value);

		virtual IClonable * Clone() const override;
	protected:
		int _value;
	};

	class BoolProperty : 
		public PropertyImpl, 
		public IBoolean,
		public IClonable
	{
	public:
		BoolProperty(const wchar_t * name, const wchar_t * description, bool value = 0);
		virtual bool GetBool();
		virtual void SetBool(bool value);

		virtual IClonable * Clone() const override;
	protected:
		bool _value;
	};

	class StringProperty : 
		public PropertyImpl, 
		public IString,
		public IClonable
	{
	public:
		StringProperty(const wchar_t * name, const wchar_t * description, const wchar_t * value = L"");
		virtual const wchar_t * GetString();
		virtual void SetString(const wchar_t * value);

		virtual IClonable * Clone() const override;
	protected:
		std::wstring _value;
	};

};

#endif // PropertyImp_h__