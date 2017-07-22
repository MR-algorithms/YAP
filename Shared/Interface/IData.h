#pragma once

#ifndef IDATA_H_20170626
#define IDATA_H_20170626

#include "smartPtr.h"
#include <complex>

#ifndef OUT
#define OUT
#endif

namespace Yap
{
	/**
		@defgroup data_type_constants Constants used to specify data type.
		Constants used to specify the data type of elements in data objects used in Yap pipelines, 
		and struct templates used to map a type to its corresponding DataType constants.
		@{
	*/
	const int DataTypeUnknown = 0x00000000;			///< Type not initialized yet.
	const int DataTypeChar = 0x00000001;			///< char (1 byte)
	const int DataTypeUnsignedChar = 0x00000002;	///< unsigned char (1 byte)
	const int DataTypeShort = 0x00000004;			///< short (2 bytes)
	const int DataTypeUnsignedShort = 0x00000008;	///< unsigned short (2 bytes)
	const int DataTypeFloat = 0x00000010;			///< float (4 bytes)
	const int DataTypeDouble = 0x00000020;			///< double (8 bytes)
	const int DataTypeInt = 0x00000040;				///< int (4 bytes)
	const int DataTypeUnsignedInt = 0x00000080;		///< unsigned int (4 bytes)
	const int DataTypeComplexFloat = 0x00000100;	///< complex<float> (2 * 4 bytes)
	const int DataTypeComplexDouble = 0x00000200;	///< complex<double> (2 * 8 bytes)
	const int DataTypeBool = 0x00000400;			///< bool (1 bytes)
	const int DataTypeAll = 0xFFFFFFFF;				///< all types accepted

	/**
	struct used to map a type to its corresponding DataType constant.
	*/
	template<typename T> struct data_type_id
	{
		static const int type = 0;
		// Don't define the above member so that DataObject can only be instantiated with
		// the following types:
	};

	template <> struct data_type_id<double> { static const int type = DataTypeDouble; };
	template <> struct data_type_id<char> { static const int type = DataTypeChar; };
	template <> struct data_type_id<unsigned char> { static const int type = DataTypeUnsignedChar; };
	template <> struct data_type_id<short> { static const int type = DataTypeShort; };
	template <> struct data_type_id<unsigned short> { static const int type = DataTypeUnsignedShort; };
	template <> struct data_type_id<float> { static const int type = DataTypeFloat; };
	template <> struct data_type_id<unsigned int> { static const int type = DataTypeUnsignedInt; };
	template <> struct data_type_id<int> { static const int type = DataTypeInt; };
	template <> struct data_type_id<std::complex<float>> { static const int type = DataTypeComplexFloat; };
	template <> struct data_type_id<std::complex<double>> { static const int type = DataTypeComplexDouble; };
	template <> struct data_type_id<bool> { static const int type = DataTypeBool; };
	/** @} */


	/**
	@brief Used to define meaning of a specific dimension of medical image data.

	MRI images or raw data are 
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

	struct IGeometry : public ISharedObject
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
		// virtual IGeometry * GetGeometry() = 0;
	};

	template <typename T> struct IDataArray : public IData
	{
		static_assert(data_type_id<T>::type != 0, "You have to use standard types.");
		/// 返回数组的起始地址。
		virtual T * GetData() = 0;
	};

};

#endif // IDATA_H_20170626