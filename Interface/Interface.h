#pragma once

enum DataType
{
	DataTypeChar,			///< wchar_t (1)
	DataTypeUnsignedChar,	///< unsigned wchar_t (1)
	DataTypeShort,			///< short (2)
	DataTypeUnsignedShort,	///< unsigned short (2)
	DataTypeFloat,			///< float (4)
	DataTypeDouble,			///< double (8)
	DataTypeInt,			///< int (4)
	DataTypeUnsignedInt,	///< unsigned int (4)
	DataTypeComplexFloat,	///< complex<float> (8)
	DataTypeComplexDouble,	///< complex<double> (16)
	DataTypeUnknown,        /// 若为此类型，说明此处理器对什么类型都接受，
							/// 若在link过程中该处理器为unknown数据类型，则将该处理器前一个数据类型和后一个处理器的数据类型进行比较。

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
	//DimensionPhase, // ICE中有，但目前不知道啥含义

	DimensionUser1,
	DimensionUser2,
	DimensionUser3,
	DimensionUser4,
	DimensionUser5,
	DimensionUser6,
	DimensionInvalid = 100,
};

struct Dimension
{
	DimensionType type;
	unsigned int start_index;
	unsigned int length;
};

struct IDimensions
{
	virtual unsigned int GetDimensionCount() = 0;
	virtual bool GetDimensionInfo(unsigned int index, OUT DimensionType& dimension_type,
		OUT unsigned int& start_index, OUT unsigned int& length) = 0;
};

struct IData
{
	virtual void * GetData() = 0;
	virtual IDimensions * GetDimension() = 0;
};

struct IProcessor
{
	virtual wchar_t * GetId() = 0;
	virtual IPortEnumerator * GetInputPortEnumerator() = 0;
	virtual IPortEnumerator * GetOutputPortEnumerator() = 0;

	virtual bool Init() = 0;
	// virtual bool Process() = 0;
	virtual bool Input(const wchar_t * port, IData * data) = 0;
	virtual bool Link(const wchar_t * out_port, IProcessor* next_processor, wchar_t * in_port) = 0;
};


struct IProcessorManager
{
	virtual IProcessor * GetFirstProcessor() = 0;
	virtual IProcessor * GetNextProcessor() = 0;
	virtual IProcessor * GetProcessor(const wchar_t * name) = 0;
};

