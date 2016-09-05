#ifndef DATAHELPER_H_20160813
#define DATAHELPER_H_20160813

#include "Interface/IData.h"

namespace Yap
{
	class CDataHelper
	{
	public:
		CDataHelper(IData * data) : _data_interface(*data) {}

		int GetDataType();
		unsigned int GetDimensionCount();
		/// �������ݵ�ʵ��ά����
		/**
		���磺ά����Ϣ��ʾΪReadout 0�� 255��phaseencoding 0�� 255��slice 0, 1��channel 0��1;
		ʵ�����ݵ�ά��ֻ�� 2����ά����Ϣ��slice 0, 2;�������䣬��ʵ�����ݵ�ά��Ϊ3���Դ����ơ�
		*/
		unsigned int GetActualDimensionCount() const;

		Dimension GetDimension(DimensionType dimension_type);
		unsigned int GetWidth();
		unsigned int GetHeight();

		size_t GetDataSize() const;
		unsigned int GetBlockSize(DimensionType type) const;

		unsigned int GetSlice();
		unsigned int GetCoilCount();
		unsigned int GetDim4();

	protected:
		IData & _data_interface;
	};

	template <typename T>
	T * GetDataArray(IData * data)
	{
		auto * data_array = dynamic_cast<IDataArray<T> *>(data);
		return (data_array != nullptr) ? data_array->GetData() : nullptr;
	}

} // namespace Yap.


#endif