#ifndef IData_h__20160825
#define IData_h__20160825

#pragma once
#ifndef OUT
#define OUT
#define IN
#endif

#include <complex>

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
	const int DataTypeAll			= 0xFFFFFFFF;	///< ��Ϊ�����ͣ�˵���˴�������ʲô���Ͷ����ܡ�

	template<typename T> struct type_id
	{
		static const int type;
	};

	template <> struct type_id<double> { static const int type = DataTypeDouble; };
	template <> struct type_id<char> { static const int type = DataTypeChar; };
	template <> struct type_id<unsigned char> { static const int type = DataTypeUnsignedChar; };
	template <> struct type_id<short> { static const int type = DataTypeShort; };
	template <> struct type_id <unsigned short> { static const int type = DataTypeUnsignedShort; };
	template <> struct type_id <float> { static const int type = DataTypeFloat; };
	template <> struct type_id <unsigned int> { static const int type = DataTypeUnsignedInt; };
	template <> struct type_id <int> { static const int type = DataTypeInt; };
	template <> struct type_id <std::complex<float>> { static const int type = DataTypeComplexFloat; };
	template <> struct type_id <std::complex<double>> { static const int type = DataTypeComplexFloat; };
	template <> struct type_id <bool> { static const int type = DataTypeBool; };


	/// �����˴Ź���ͼ�������еĳ��õ�ά�ȡ�
	/**
	ͨ���ڶ����ά����ʱ��Readout, PhaseEncoding, Slice������������������洢��
	����ά�ȵĴ�����ȷ����ϰ��������������ά����Ϣʱ��Ӧ����ϰ�ߵ�����������
	*/
	enum DimensionType
	{
		DimensionReadout,
		DimensionPhaseEncoding,
		DimensionSlice,
		Dimension4,				// Ϊ�˼����Կ��ǣ����岻��ȷ
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

	struct IDimensions
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

	struct IData
	{
		/// �����������ʼ��ַ��
		/** ������ݺ�������ǿ������ת����ʹ��. */
		virtual void * GetData() = 0;

		virtual int GetDataType() = 0;			///< ��������Ԫ�ص�����
		virtual IDimensions * GetDimensions() = 0;	///< ������ݵ�ά����Ϣ��
	};
}
#endif // IData_h__
