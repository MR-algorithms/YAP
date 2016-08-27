#pragma once

#ifndef yapParam_h__20160818
#define yapParam_h__20160818

namespace Yap
{
	enum PropertyType
	{
		PropertyBool,
		PropertyInt,
		PropertyFloat,
		PropertyString,
		PropertyStruct,
		PropertyArray,
	};

	struct IProperty
	{
		virtual const wchar_t * GetName() = 0;
		virtual PropertyType GetType() = 0;
		virtual const wchar_t * GetDescription() = 0;
	};

	struct IPropertyEnumerator
	{
		virtual IProperty * GetFirst() = 0;
		virtual IProperty * GetNext() = 0;
		virtual IProperty * GetProperty(const wchar_t * name) = 0;
	};

	struct IFloatValue
	{
		virtual double GetValue() = 0;
		virtual void SetValue(double value) = 0;
	};

	struct IBoolValue
	{
		virtual bool GetValue() = 0;
		virtual void SetValue(bool value) = 0;
	};

	struct IIntValue
	{
		virtual int GetValue() = 0;
		virtual void SetValue(int value) = 0;
	};

	struct IStringValue
	{
		virtual const wchar_t * GetValue() = 0;
		virtual void SetValue(const wchar_t * value) = 0;
	};

};

#endif // yapParam_h__