#pragma once
#ifndef OUT
#define OUT
#define IN
#endif

namespace Yap
{
	enum DataType
	{
		DataTypeUnknown			= 0x00000000,	///< enum not initialized yet.
		DataTypeChar			= 0x00000001,	///< wchar_t (1)
		DataTypeUnsignedChar	= 0x00000002,	///< unsigned wchar_t (1)
		DataTypeShort			= 0x00000004,	///< short (2)
		DataTypeUnsignedShort	= 0x00000008,	///< unsigned short (2)
		DataTypeFloat			= 0x00000010,	///< float (4)
		DataTypeDouble			= 0x00000020,	///< double (8)
		DataTypeInt				= 0x00000040,	///< int (4)
		DataTypeUnsignedInt		= 0x00000080,	///< unsigned int (4)
		DataTypeComplexFloat	= 0x00000100,	///< complex<float> (8)
		DataTypeComplexDouble	= 0x00000200,	///< complex<double> (16)
		DataTypeBool			= 0x00000400,	///< bool
		DataTypeAll				= 0xFFFFFFFF,	///< 若为此类型，说明此处理器对什么类型都接受。
	};

	struct IPort
	{
		virtual const wchar_t * GetName() = 0;
		virtual unsigned int GetDimensions() = 0;
		virtual DataType GetDataType() = 0;
	};

	struct IPortEnumerator
	{
		virtual IPort * GetFirstPort() = 0;
		virtual IPort * GetNextPort() = 0;
		virtual IPort * GetPort(const wchar_t * name) = 0;
	};

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

	enum PropertyType
	{
		PropertyBool,
		PropertyInt,
		PropertyFloat,
		PropertyString,
	};

	const unsigned int YAP_ANY_DIMENSION = 0xffffffff;

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

	struct IDimensions
	{
		virtual unsigned int GetDimensionCount() = 0;
		virtual bool GetDimensionInfo(unsigned int index, OUT DimensionType& dimension_type,
			OUT unsigned int& start_index, OUT unsigned int& length) = 0;
	};

	struct IMemory
	{
		virtual void Lock() = 0;
		virtual void Release() = 0;
	};

	struct ILocalization
	{
		virtual ILocalization* Clone() = 0;
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

	struct IData
	{
		/// 返回数组的起始地址。
		/** 获得数据后必须进行强制类型转换后使用. */
		virtual void * GetData() = 0;

		virtual DataType GetDataType() = 0;			///< 返回数据元素的类型
		virtual IDimensions * GetDimensions() = 0;	///< 获得数据的维度信息。
	};

	struct IProcessor
	{
		/// 复制当前Processor的实例。
		virtual IProcessor * Clone() = 0;
		/// 释放当前Processor的资源。
		virtual void Release() = 0;

		virtual const wchar_t * GetClassId() = 0;
		virtual void SetClassId(const wchar_t * id) = 0;
		virtual const wchar_t * GetInstanceId() = 0;
		virtual void SetInstanceId(const wchar_t * instance_id) = 0;

		virtual IPortEnumerator * GetInputPortEnumerator() = 0;
		virtual IPortEnumerator * GetOutputPortEnumerator() = 0;

		/// 获得属性访问接口。
		virtual IPropertyEnumerator * GetProperties() = 0;

		/// 将指定名称的属性与参数空间的参数相关联。
		virtual bool LinkProperty(const wchar_t * property_id, const wchar_t * param_id) = 0;

		/// 接口用户调用这个函数来通知模块利用参数空间中的参数更新属性。
		virtual bool UpdateProperties(IPropertyEnumerator * params) = 0;

		/// 将指定处理模块的输入端口链接到当前模块指定的输出端口上。
		virtual bool Link(const wchar_t * output, IProcessor * next, const wchar_t * next_input) = 0;

		/// 向当前处理模块馈送数据。
		virtual bool Input(const wchar_t * name, IData * data) = 0;
	};

	struct IProcessorManager
	{
		virtual IProcessor * GetFirstProcessor() = 0;
		virtual IProcessor * GetNextProcessor() = 0;
		virtual IProcessor * GetProcessor(const wchar_t * name) = 0;
		virtual void Release() = 0;
	};
}
