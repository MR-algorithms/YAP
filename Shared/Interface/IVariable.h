#pragma once

#ifndef IVariable_h__20170803
#define IVariable_h__20170803

#include "smartptr.h"
#include "IContainer.h"

#include <type_traits>
#include <string>
#include <vector>

namespace Yap
{
	/** @defgroup Bit flags for PropertyType
	@{ */
	const int VariableInvalid       = 0x0000;
	const int VariableBool          = 0x0001;
	const int VariableInt           = 0x0002;
	const int VariableFloat         = 0x0004;
	const int VariableString        = 0x0008;
	const int VariableStruct        = 0x0010;
	const int VariableBoolArray		= 0x0200;
	const int VariableIntArray		= 0x0400;
	const int VariableFloatArray	= 0x0800;
	const int VariableStringArray	= 0x1000;
	const int VariableStructArray	= 0x2000;
	const int VariableAllTypes      = 0xffffffff;
	/** @} */

	template<typename T> struct variable_type_id
	{
		static const int type_id;
		static const int array_type_id;
		typedef T type;
		typedef T set_type;
		typedef T get_type;
		typedef std::vector<T> vector_type;
	};

	template <> struct variable_type_id<bool>
	{
		static const int type_id = VariableBool;
		static const int array_type_id = VariableBoolArray;
		typedef bool type;
		typedef bool set_type;
		typedef bool get_type;
		typedef std::vector<unsigned char> vector_type;
	};

	template <> struct variable_type_id<int>
	{
		static const int type_id = VariableInt;
		static const int array_type_id = VariableIntArray;
		typedef int set_type;
		typedef int get_type;
		typedef int type;
		typedef std::vector<int> vector_type;
	};

	template <> struct variable_type_id<double>
	{
		static const int type_id = VariableFloat;
		static const int array_type_id = VariableFloatArray;
		typedef double set_type;
		typedef double get_type;
		typedef double type;
		typedef std::vector<double> vector_type;
	};

	template <> struct variable_type_id<std::wstring>
	{
		static const int type_id = VariableString;
		static const int array_type_id = VariableStringArray;

		typedef std::wstring type;
		typedef const wchar_t * const set_type;
		typedef const wchar_t * const get_type;
		typedef std::vector<std::wstring> vector_type;
	};


	/**
	@brief Interface for a variable.
	Variables are used in YAP to represent properties of processors and system parameters.
	Variables are similar to variables in C++, only they are all implemented as objects of
	classes implementing IVariable and derived interfaces.
	*/
	struct IVariable : public ISharedObject
	{
		/**
		@brief Returns the type of the variable.
		*/
		virtual int GetType() const = 0;
		virtual const wchar_t * GetId() const = 0;
		virtual void SetId(const wchar_t * id) = 0;
		virtual void SetTitle(const wchar_t * title) = 0;
		virtual const wchar_t * GetTitle() const = 0;
		virtual const wchar_t * GetDescription() const = 0;
		virtual void SetDescription(const wchar_t * description) = 0;

		/** @brief Enable a variable.
		Normally, a disabled UI variable is disabled or even hidden in the UI, and
		a disabled spectrometer variable will not be transfered to the spectrometer.
		*/
		virtual void Enable(bool enable) = 0;

		/** @brief Check to see if the variable is enabled or not.
		See Enable().
		*/
		virtual bool IsEnabled() const = 0;

		/** @brief Store the value of this variable to a string, using JSON-like style.
		@remark Examples of stored string:
		# integer or float numerical literals;
		# "strings literals";
		# boolean: true / false
		# array: [element0, element1, ..., element_n]
		# struct: {"field_id_1" : value1, "field_id_2" : value2, ..., "field_id_n" : value_n}

		If the variable has a nested structure, the stored value string is also nested.
		*/
		virtual const wchar_t * const ToString() = 0;

		/** @brief Extract variable from the string.
		@return Number of characters extracted from the string.
		@remark This function need not consume all characters in the string,
		because this function may be called recursively from implement classes.
		The caller should check the return value if all characters need to be
		consumed.
		*/
		virtual size_t FromString(const wchar_t * value_string) = 0;
	};

	template <> struct variable_type_id <IVariable*> 
	{
		static const int type_id = VariableInvalid;
		static const int array_type_id = VariableStructArray;
		typedef SmartPtr<IVariable> type;
		typedef std::vector<type> vector_type;
	};

	template<typename VALUE_TYPE>
	struct ISimpleVariable : public IVariable
	{
		typedef typename variable_type_id<VALUE_TYPE>::get_type get_type;
		typedef typename variable_type_id<VALUE_TYPE>::set_type set_type;

		virtual get_type Get() const = 0;
		virtual void Set(set_type value) = 0;
	};

	struct IEnumItem
	{
		virtual int32_t GetValue() const = 0;
		virtual const wchar_t * const GetItemString() const = 0;
		virtual const wchar_t * const GetDescription() const = 0;
	};

	struct IEnumType
	{
		virtual const wchar_t * const GetTypeId() const = 0;
		virtual const wchar_t * const GetDescription() const = 0;
		virtual uint32_t GetEnumValueCount() const = 0;
		virtual const IEnumItem * GetEnumItem(uint32_t index) const = 0;
	};

	struct IEnumVariable : public IVariable
	{
		virtual int Get() const = 0;
		virtual bool Set(int value) = 0;
		virtual const IEnumType * GetEnumType() = 0;
	};

	struct IArrayBase : public IVariable
	{
		virtual size_t GetSize() const = 0;
		virtual void SetSize(size_t size) = 0;
	};

	template <typename T>
	struct IValueArray : public IArrayBase
	{
		typedef typename variable_type_id<T>::vector_type vector_type;
		typedef typename variable_type_id<T>::type type;
		typedef typename variable_type_id<T>::get_type get_type;
		typedef typename variable_type_id<T>::set_type set_type;

		static_assert(std::is_same<T, int>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, bool>::value ||
			std::is_same<T, std::wstring>::value ||
			std::is_same<T, typename SmartPtr<IVariable>>::value ||
			std::is_base_of<IVariable, typename std::remove_pointer<T>::type>::value,
			"You can only use one of the following types: int, double, bool, std::wstring, IVariable*");

		virtual get_type Get(size_t index) const = 0;
		virtual void Set(size_t index, set_type value) = 0;
		virtual type GetElementTemplate() const = 0;
	};

	template <typename T>
	struct IElementReference
	{
		static_assert(std::is_same<T, int>::value ||
			std::is_same<T, double>::value ||
			std::is_base_of<IVariable, typename std::remove_pointer<T>::type>::value ||
			std::is_same<T, typename SmartPtr<IVariable>>::value, 
			"You can only use one of the following types: int, double, IVariable*");

		virtual T& Element(size_t index) = 0;
	};

	template <typename T>
	struct IRawArray
	{
		static_assert(std::is_same<T, int>::value ||
			std::is_same<T, double>::value,
			"You can only use one of the following types: int, double");
		virtual T * Data() = 0;
	};

	typedef IPtrContainer<IVariable> IVariableContainer;
	typedef IVariableContainer::Iterator IVariableIter;

	struct IStructVariable : public IVariable
	{
		virtual IVariableContainer * Members() = 0;
		virtual IStructVariable* Clone() const = 0;
	};
}

#endif // IVariable_h__
