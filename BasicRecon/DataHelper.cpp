#include "stdafx.h"
#include "DataHelper.h"
#include "..\Utilities\macros.h"
#include "..\Interface\DataImp.h"

using namespace Yap;

DataType CDataHelper::GetDataType()
{
	return _data_interface.GetDataType();
}

unsigned int CDataHelper::GetDimensionCount()
{
	auto dimension = _data_interface.GetDimensions();

	return (dimension != nullptr) ? dimension->GetDimensionCount() : 0;
}

unsigned int CDataHelper::GetWidth()
{
	auto dimension = _data_interface.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;
	dimension->GetDimensionInfo(0, dimension_type, start, length);

	return length;
}

unsigned int CDataHelper::GetHeight()
{
	auto dimension = _data_interface.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;
	dimension->GetDimensionInfo(1, dimension_type, start, length);

	return length;
}

unsigned int CDataHelper::GetSlice()
{
	auto dimension = _data_interface.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;
	dimension->GetDimensionInfo(2, dimension_type, start, length);
	return length;
}

void * CDataHelper::GetData()
{
	return _data_interface.GetData();
}

Yap::Dimension Yap::CDataHelper::GetDimension(DimensionType dimension_type)
{
	assert(_data_interface.GetDimensions());

	Dimension dimension;
	unsigned int dimension_count = _data_interface.GetDimensions()->GetDimensionCount();
	for (unsigned int dimension_index = 0; dimension_index < dimension_count; ++dimension_index)
	{
		_data_interface.GetDimensions()->GetDimensionInfo(dimension_index,
			dimension.type, dimension.start_index, dimension.length);
		if (dimension.type == dimension_type)
		{
			break;
		}
	}

	return dimension;
}

size_t Yap::CDataHelper::GetDataSize() const
{
	size_t count = 1;
	auto dimension = _data_interface.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;

	for (unsigned int i = 0; i < dimension->GetDimensionCount(); ++i)
	{
		dimension->GetDimensionInfo(i, dimension_type, start, length);
		count *= length;
	}

	return count;
}

unsigned int CDataHelper::GetCoilCount()
{
	auto dimension = _data_interface.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;

	TODO(Review the code.);

	dimension->GetDimensionInfo(4, dimension_type, start, length);

	return length;
}

/**
获得指定维度的一块数据所包含的数据元素的个数。例如当type等于DimensionSlice时，
返回一个slice中的元素个数。
@param  type 指定所关心的维度。
@return 包含的数据元素的个数。
*/
unsigned int CDataHelper::GetBlockSize(DimensionType type) const
{
	assert(_data_interface.GetDimensions() != nullptr);

	Dimension dimension;
	unsigned int dimension_count = _data_interface.GetDimensions()->GetDimensionCount();
	unsigned int block_size = 1;

	bool success = false;
	for (unsigned int dimension_index = 0; dimension_index < dimension_count; ++dimension_index)
	{
		_data_interface.GetDimensions()->GetDimensionInfo(dimension_index,
			dimension.type, dimension.start_index, dimension.length);

		if (dimension.type == type)
		{
			success = true;
			break;
		}
		block_size *= dimension.length;
	}

	return block_size;
}