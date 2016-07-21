#pragma once
#include "..\Interface\YapInterfaces.h"
#include "../Interface/DataImp.h"

namespace Yap
{
	class CDataHelper
	{
	public:
		CDataHelper(IData * data) : _data_interface(*data) {}

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

		void * GetData();
		size_t GetDataSize() const;
		unsigned int GetBlockSize(DimensionType type) const;

		unsigned int GetSlice();
		unsigned int GetCoilCount();
		unsigned int GetDim4();

	protected:
		IData & _data_interface;
	};
}
