#pragma once
#include "..\Interface\YapInterfaces.h"
#include "../Interface/DataImp.h"

namespace Yap
{
	class CDataHelper
	{
	public:
		CDataHelper(IData * data) : _data_interface(*data) {}

		DataType GetDataType();
		unsigned int GetDimensionCount();
		Dimension GetDimension(DimensionType dimension_type);
		unsigned int GetWidth();
		unsigned int GetHeight();

		void * GetData();
		size_t GetDataSize() const;
		unsigned int GetBlockSize(DimensionType type) const;

		unsigned int GetSlice();
		unsigned int GetCoilCount();

	protected:
		IData & _data_interface;
	};
}
