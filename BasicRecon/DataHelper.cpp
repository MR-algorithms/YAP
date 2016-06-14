#include "stdafx.h"
#include "DataHelper.h"

DataType CData::GetDataType()
{
	return _data_interface.GetDataType();
}

unsigned int CData::GetDimensionCount()
{
	auto dimension = _data_interface.GetDimension();

	return (dimension != nullptr) ? dimension->GetDimensionCount() : 0;
}

unsigned int CData::GetWidth()
{
	auto dimension = _data_interface.GetDimension();
	DimensionType dimension_type;
	unsigned int start, length;
	dimension->GetDimensionInfo(0, dimension_type, start, length);

	return length;
}

unsigned int CData::GetHeight()
{
	auto dimension = _data_interface.GetDimension();
	DimensionType dimension_type;
	unsigned int start, length;
	dimension->GetDimensionInfo(1, dimension_type, start, length);

	return length;
}

void * CData::GetData()
{
	return _data_interface.GetData();
}

