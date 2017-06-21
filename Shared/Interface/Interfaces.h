#ifndef IData_h__20160825
#define IData_h__20160825

#pragma once
#ifndef OUT
#define OUT
#define IN
#endif

#include <complex>
#include "smartptr.h"

namespace Yap
{
	const int DataTypeUnknown		= 0x00000000;	///< enum not initialized yet.
	const int DataTypeChar			= 0x00000001;	///< wchar_t (1)
	const int DataTypeUnsignedChar	= 0x00000002;	///< unsigned wchar_t (1)
	const int DataTypeShort			= 0x00000004;	///< short (2)
	const int DataTypeUnsignedShort	= 0x00000008;	///< unsigned short (2)
	const int DataTypeFloat			= 0x00000010;	///< float (4)
	const int DataTypeDouble		= 0x00000020;	///< double (8)
	const int DataTypeInt			= 0x00000040;	///< int (4)
	const int DataTypeUnsignedInt	= 0x00000080;	///< unsigned int (4)
	const int DataTypeComplexFloat	= 0x00000100;	///< complex<float> (8)
	const int DataTypeComplexDouble	= 0x00000200;	///< complex<double> (16)
	const int DataTypeBool			= 0x00000400;	///< bool
	const int DataTypeAll			= 0xFFFFFFFF;	///< 若为此类型，说明此处理器对什么类型都接受。

	template<typename T> struct data_type_id
	{
		static const int type;
	};

	template <> struct data_type_id<double> { static const int type = DataTypeDouble; };
	template <> struct data_type_id<char> { static const int type = DataTypeChar; };
	template <> struct data_type_id<unsigned char> { static const int type = DataTypeUnsignedChar; };
	template <> struct data_type_id<short> { static const int type = DataTypeShort; };
	template <> struct data_type_id <unsigned short> { static const int type = DataTypeUnsignedShort; };
	template <> struct data_type_id <float> { static const int type = DataTypeFloat; };
	template <> struct data_type_id <unsigned int> { static const int type = DataTypeUnsignedInt; };
	template <> struct data_type_id <int> { static const int type = DataTypeInt; };
	template <> struct data_type_id <std::complex<float>> { static const int type = DataTypeComplexFloat; };
	template <> struct data_type_id <std::complex<double>> { static const int type = DataTypeComplexDouble; };
	template <> struct data_type_id <bool> { static const int type = DataTypeBool; };


	/// 定义了磁共振图像数据中的常用的维度。
	/**
	通常在定义多维数据时，Readout, PhaseEncoding, Slice三个方向会优先连续存储。
	其他维度的次序则不确定。习惯上在描述数据维度信息时，应按照习惯的名称命名。
	*/
	enum DimensionType
	{
		DimensionReadout,
		DimensionPhaseEncoding,
		DimensionSlice,
		Dimension4,				// 为了兼容性考虑，意义不明确
		DimensionChannel,
		DimensionAverage,
		DimensionSlab,
		DimensionEcho,
		DimensionPhase,

		DimensionUser1,
		DimensionUser2,
		DimensionUser3,
		DimensionUser4,
		DimensionUser5,
		DimensionUser6,
		DimensionInvalid = 100,
	};

	const unsigned int YAP_ANY_DIMENSION = 0xffffffff;

	struct Dimension
	{
		DimensionType type;
		unsigned int start_index;
		unsigned int length;
		Dimension();
		Dimension(DimensionType type, unsigned start_index, unsigned int length);
	};

	struct IDimensions : public ISharedObject
	{
		virtual unsigned int GetDimensionCount() = 0;
		virtual bool GetDimensionInfo(unsigned int index, OUT DimensionType& dimension_type,
			OUT unsigned int& start_index, OUT unsigned int& length) = 0;
	};

	struct IGeometry
	{
		virtual void GetSpacing(double& x, double& y, double& z) = 0;
		virtual void GetRowVector(double& x, double& y, double& z) = 0;
		virtual void GetColumnVector(double& x, double& y, double& z) = 0;
		virtual void GetSliceVector(double& x, double& y, double& z) = 0;
		virtual void GetReadoutVector(double& x, double& y, double& z) = 0;
		virtual void GetPhaseEncodingVector(double& x, double& y, double& z) = 0;
		virtual void GetCenter(double& x, double& y, double& z) = 0;

		virtual void SetSpacing(double x, double y, double z) = 0;
		virtual void SetRowVector(double x, double y, double z) = 0;
		virtual void SetColumnVector(double x, double y, double z) = 0;
		virtual void SetSliceVector(double x, double y, double z) = 0;
		virtual void SetReadoutVector(double x, double y, double z) = 0;
		virtual void SetPhaseEncodingVector(double x, double y, double z) = 0;
		virtual void SetCenter(double x, double y, double z) = 0;

		virtual bool IsValid() = 0;
	};

	struct IData : public ISharedObject
	{
		virtual int GetDataType() = 0;				///< 返回数据元素的类型
		virtual IDimensions * GetDimensions() = 0;	///< 获得数据的维度信息。
	};

	template <typename T> struct IDataArray : public IData
	{
		/// 返回数组的起始地址。
		virtual T * GetData() = 0;
	};

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

	// bit flags for PropertyType
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

	struct IVariable : public ISharedObject
	{
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
		virtual T Get(size_t index) const = 0;
		virtual void Set(size_t index, T value) = 0;
	};

	template <typename T>
	struct IElementReference
	{
		virtual T& Element(size_t index) = 0;
	};

	template <typename T>
	struct IRawArray
	{
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
