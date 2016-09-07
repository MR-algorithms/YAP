#ifndef DATAHELPER_H_20160813
#define DATAHELPER_H_20160813

#include "Interface/IData.h"

namespace Yap
{
	class DataHelper
	{
	public:
		DataHelper(IData * data) : _data_object(*data) {}

		int GetDataType();
		unsigned int GetDimensionCount();
		/// 计算数据的实际维数。
		/**
		例如：维度信息表示为Readout 0， 255；phaseencoding 0， 255；slice 0, 1；channel 0，1;
		实际数据的维度只是 2。若维度信息中slice 0, 2;其它不变，则实际数据的维度为3，以此类推。
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
		IData & _data_object;
	};

	template <typename T>
	T * GetDataArray(IData * data)
	{
		auto * data_array = dynamic_cast<IDataArray<T> *>(data);
		return (data_array != nullptr) ? data_array->GetData() : nullptr;
	}

} // namespace Yap.


#endif