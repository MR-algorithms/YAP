#pragma once

#ifndef yapParam_h__20160818
#define yapParam_h__20160818

#include "IIterator.h"

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
		virtual const wchar_t * GetDescription() = 0;
		virtual PropertyType GetType() = 0;
	};

	typedef IIterator<IProperty> IPropertyIter;

	struct IPropertyContainer
	{
		virtual IPropertyIter * GetIterator() = 0;
		virtual IProperty * GetProperty(const wchar_t * name) = 0;
	};

	struct IDouble
	{
		virtual double GetDouble() = 0;
		virtual void SetDouble(double value) = 0;
	};

	struct IBoolean
	{
		virtual bool GetBool() = 0;
		virtual void SetBool(bool value) = 0;
	};

	struct IInt
	{
		virtual int GetInt() = 0;
		virtual void SetInt(int value) = 0;
	};

	struct IString
	{
		virtual const wchar_t * GetString() = 0;
		virtual void SetString(const wchar_t * value) = 0;
	};

};

#endif // yapParam_h__