#ifndef Interface_h__20160825
#define Interface_h__20160825

#pragma once
#ifndef OUT
#define OUT
#define IN
#endif

#include "idata.h"

#include <complex>
#include "smartptr.h"
#include <type_traits>

namespace Yap
{
	template <typename ELEMENT_TYPE>
	struct IIterator
	{
		virtual ELEMENT_TYPE * GetFirst() = 0;
		virtual ELEMENT_TYPE * GetNext() = 0;
	};

	template <typename ELEMENT_TYPE>
	struct IPtrContainer : public ISharedObject
	{
		typedef IIterator<ELEMENT_TYPE> iterator;

		virtual ELEMENT_TYPE * Find(const wchar_t * name) = 0;
		virtual iterator * GetIterator() = 0;
		virtual bool Add(const wchar_t * name, ELEMENT_TYPE * element) = 0;
		virtual bool Delete(const wchar_t * name) = 0;
		virtual void Clear() = 0;
	};

	struct IProcessor;
	typedef IPtrContainer<IProcessor> IProcessorContainer;
	typedef IProcessorContainer::iterator IProcessorIter;

	/** @defgroup Bit flags for PropertyType
	@{ */
	const int VariableInvalid = 0;					
	const int VariableBool = 1;
	const int VariableInt = 2;
	const int VariableFloat = 4;
	const int VariableString = 8;
	const int VariableStruct = 16;
	const int VariableBoolArray = 32;
	const int VariableIntArray = 64;
	const int VariableFloatArray = 128;
	const int VariableStringArray = 256;
	const int VariableStructArray = 512;
    const int VariableAllTypes = 0xffffffff;
	/** @} */

	template<typename T> struct variable_type_id
	{
		static const int type_id;
		static const int array_type_id;
	};

	template <> struct variable_type_id<bool>
	{
		static const int type_id = VariableBool;
		static const int array_type_id = VariableBoolArray;
	};

	template <> struct variable_type_id<int>
	{
		static const int type_id = VariableInt;
		static const int array_type_id = VariableIntArray;
	};

	template <> struct variable_type_id<double>
	{
		static const int type_id = VariableFloat;
		static const int array_type_id = VariableFloatArray;
	};

	template <> struct variable_type_id<const wchar_t * const>
	{
		static const int type_id = VariableString;
		static const int array_type_id = VariableStringArray;
	};

	template <> struct variable_type_id<wchar_t *>
	{
		static const int type_id = VariableString;
		static const int array_type_id = VariableStringArray;
	};

	template <> struct variable_type_id<wchar_t const *>
	{
		static const int type_id = VariableString;
		static const int array_type_id = VariableStringArray;
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
			# struct: {"field_id_1" : value1, "field_id_2", value2, ..., "field_id_n", value_n}
			
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

	template <> struct variable_type_id <IVariable*> {
		static const int type_id = VariableInvalid;
		static const int array_type_id = VariableStructArray;
	};

	typedef IPtrContainer<IVariable> IVariableContainer;
	typedef IVariableContainer::iterator IVariableIter;

	// ================== new variable interfaces ========================

	template<typename VALUE_TYPE>
	struct ISimpleVariable : public IVariable
	{
		virtual VALUE_TYPE Get() const = 0;
		virtual void Set(VALUE_TYPE value) = 0;
	};

	struct IArrayBase : public IVariable
	{
		virtual size_t GetSize() const = 0;
		virtual void SetSize(size_t size) = 0;
	};

	template <typename T>
	struct IValueArrayVariable : public IArrayBase
	{
		static_assert(std::is_same<T, int>::value ||
			std::is_same<T, double>::value ||
			std::is_same<T, bool>::value ||
			std::is_same<T, const wchar_t * const>::value ||
			std::is_base_of<IVariable, typename std::remove_pointer<T>::type>::value,
			"You can only use one of the following types: int, double, bool, const wchar_t * const, IVariable*");

		virtual T Get(size_t index) const = 0;
		virtual void Set(size_t index, T value) = 0;
	};

	template <typename T>
	struct IElementReference
	{
		static_assert(std::is_same<T, int>::value ||
			std::is_same<T, double>::value ||
			std::is_base_of<IVariable, typename std::remove_pointer<T>::type>::value,
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

	struct IStructVariable : public IVariable
	{
		virtual IPtrContainer<IVariable> * Members() = 0;
	};

    typedef IPtrContainer<IVariable> IStructValue;

	typedef IValueArrayVariable<IStructVariable*> IStructArray;

	// ================== end new variable interfaces ====================

	struct IPort : public ISharedObject
	{
		virtual const wchar_t * GetId() const = 0;
		virtual unsigned int GetDimensionCount() const = 0;
		virtual int GetDataType() const = 0;
	};

	typedef IPtrContainer<IPort> IPortContainer;
	typedef IPortContainer::iterator IPortIter;

	struct IProcessor : public ISharedObject
	{
		virtual ISharedObject * Clone() const override = 0;
		virtual const wchar_t * GetClassId() const = 0;
		virtual void SetClassId(const wchar_t * id) = 0;

		virtual const wchar_t * GetInstanceId() const = 0;
		virtual void SetInstanceId(const wchar_t * instance_id) = 0;

		virtual IPortContainer * Inputs() = 0;
		virtual IPortContainer * Outputs() = 0;

		/// Return all properties of the processor.
		virtual IVariableContainer * GetProperties() = 0;

		/// Map a global variable to a processor property.
		virtual bool MapProperty(const wchar_t * property_id, const wchar_t * variable_id,
			bool input, bool output) = 0;

		/// Set the global variable space. 
		virtual bool SetGlobalVariables(IVariableContainer * params) = 0;

		/// Line the output port to an input port of another processor.
		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) = 0;

		/// Feed data into the processor via the specified port.
		virtual bool Input(const wchar_t * name, IData * data) = 0;

		/** @brief Specifies the module that hosts this processor. @p
		The module should be locked till the processor is no longer used. */
		virtual void SetModule(ISharedObject * module) = 0;
	};

	typedef IPtrContainer<IProcessor> IProcessorContainer;
	typedef IProcessorContainer::iterator IProcessorIter;

	enum LogLevel
	{
		LevelTrace,
		LevelDebug,
		LevelInfo,
		LevelWarn,
		LevelError,
		LevelFatal,
	};

	struct ILogUser;
	struct ILog
	{
		virtual void Log(const wchar_t * module, const wchar_t * info, LogLevel level, const wchar_t * log_name = L"", bool flush = false) = 0;
		virtual void AddUser(ILogUser * user) = 0;
		virtual void RemoveUser(ILogUser * user) = 0;
	};

	struct ILogUser
	{
		virtual void SetLog(ILog * log) = 0;
	};
}
#endif // IData_h__
