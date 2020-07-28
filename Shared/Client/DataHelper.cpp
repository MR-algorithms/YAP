#include "DataHelper.h"
#include "Utilities/macros.h"

#include <cassert>

using namespace Yap;

int DataHelper::GetDataType()
{
	return _data_object.GetDataType();
}

unsigned int DataHelper::GetDimensionCount()
{
	auto dimension = _data_object.GetDimensions();

	return (dimension != nullptr) ? dimension->GetDimensionCount() : 0;
}

/**
�������ݵ�ʵ��ά�������磬������ά�ռ��е�ƽ�棬��ʵ��ά��Ӧ������ά����Ȼ���õĿռ���������ά�ġ�
@return ���ݼ���ʵ��ά����
*/
unsigned int DataHelper::GetActualDimensionCount() const
{
	assert(_data_object.GetDimensions() != nullptr);

	Dimension dimension;
	unsigned int dimension_count = _data_object.GetDimensions()->GetDimensionCount();
	unsigned int actual_dimension_count = 0;
	for (unsigned int dimension_index = 0; dimension_index < dimension_count; ++dimension_index)
	{
		_data_object.GetDimensions()->GetDimensionInfo(dimension_index,
			dimension.type, dimension.start_index, dimension.length);
		if (dimension.length > 1)
		{
			++actual_dimension_count;
		}
	}

	return actual_dimension_count;
}

unsigned int DataHelper::GetWidth()
{
	auto dimension = _data_object.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;
	dimension->GetDimensionInfo(0, dimension_type, start, length);

	return length;
}

unsigned int DataHelper::GetHeight()
{
	auto dimension = _data_object.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;
	dimension->GetDimensionInfo(1, dimension_type, start, length);

	return length;
}

unsigned int DataHelper::GetSliceCount()
{
	auto dimension = _data_object.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;
	dimension->GetDimensionInfo(2, dimension_type, start, length);
	return length;
}


Yap::Dimension Yap::DataHelper::GetDimension(DimensionType dimension_type)
{
	auto dims = _data_object.GetDimensions();
	assert(dims != nullptr);

	Dimension dimension;
	unsigned int dim_index;
	for (dim_index = 0; dim_index < dims->GetDimensionCount(); ++dim_index)
	{
		dims->GetDimensionInfo(dim_index,
			dimension.type, dimension.start_index, dimension.length);
		if (dimension.type == dimension_type)
		{
			break;
		}
	}
	if (dim_index == dims->GetDimensionCount())
	{
		dimension.type = DimensionInvalid;
		dimension.start_index = 0U;
		dimension.length = 0U;
	}
	return dimension;
}

size_t Yap::DataHelper::GetDataSize() const
{
	size_t count = 1;
	auto dimension = _data_object.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;

	for (unsigned int i = 0; i < dimension->GetDimensionCount(); ++i)
	{
		dimension->GetDimensionInfo(i, dimension_type, start, length);
		count *= length;
	}

	return count;
}

unsigned int DataHelper::GetCoilCount()
{
	auto dimension = _data_object.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;

	TODO(Review the code.);
	//�����һ����������Ϊ4�������⡣
	dimension->GetDimensionInfo(4, dimension_type, start, length);

	return length;
}

unsigned int Yap::DataHelper::GetDim4()
{
	auto dimension = _data_object.GetDimensions();
	DimensionType dimension_type;
	unsigned int start, length;

	dimension->GetDimensionInfo(3, dimension_type, start, length);

	return length;
}

Yap::IData * Yap::DataHelper::GetData()
{
	return &_data_object;
}

/**
���ָ��ά�ȵ�һ������������������Ԫ�صĸ��������統type����DimensionSliceʱ��
����һ��slice�е�Ԫ�ظ�����
@param  type ָ�������ĵ�ά�ȡ�
@return ����������Ԫ�صĸ�����
*/
unsigned int DataHelper::GetBlockSize(DimensionType type) const
{
	assert(_data_object.GetDimensions() != nullptr);

	Dimension dimension;
	unsigned int dimension_count = _data_object.GetDimensions()->GetDimensionCount();
	unsigned int block_size = 1;

	bool success = false;
	for (unsigned int dimension_index = 0; dimension_index < dimension_count; ++dimension_index)
	{
		_data_object.GetDimensions()->GetDimensionInfo(dimension_index,
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