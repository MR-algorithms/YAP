#include "DataImpl.h"

using namespace Yap;

/**
	获得相关数据使用的总的维数。
*/
unsigned int Yap::DimensionsImpl::GetDimensionCount() 
{
	return static_cast<unsigned int>(_dimension_info.size());
}

/**
	获得指定的维度的信息。
	@param  dimension_index 指定希望获得其信息的维度的索引，从0开始。
	@param  dimension_type 输出参数，指定索引的类型（意义）。
	@param  start_index 相关的数据在这个维度上的起始位置。
	@param  length 相关的数据在这个维度上的宽度。
	@return
*/
bool Yap::DimensionsImpl::GetDimensionInfo(unsigned int dimension_index, 
										   DimensionType & dimension_type, 
	unsigned int& start_index, 
												  unsigned int& length)
{
	assert(dimension_index < _dimension_info.size());
	if (dimension_index >= _dimension_info.size())
	{
		return false;
	}

	dimension_type = _dimension_info[dimension_index].type;
	start_index = _dimension_info[dimension_index].start_index;
	length = _dimension_info[dimension_index].length;

	return true;
}

IClonable * Yap::DimensionsImpl::Clone() const
{
	return new (std::nothrow) DimensionsImpl(*this);
}

DimensionsImpl & Yap::DimensionsImpl::operator()(DimensionType type, unsigned int index, unsigned int length)
{
	assert(type != DimensionInvalid);

	_dimension_info.push_back(Dimension(type, index, length));

	return *this;
}

Yap::DimensionsImpl::DimensionsImpl()
{
}

Yap::DimensionsImpl::DimensionsImpl(const DimensionsImpl& source)
{
	_dimension_info = source._dimension_info;
}

Yap::DimensionsImpl::DimensionsImpl(IDimensions * source)
{
	DimensionType type;
	unsigned int index, length;

	for (unsigned int i = 0; i < source->GetDimensionCount(); ++i)
	{
		source->GetDimensionInfo(i, type, index, length);
		_dimension_info.push_back(Dimension(type, index, length));
	}
}

Yap::DimensionsImpl::DimensionsImpl(unsigned int dimension_count)
{
	assert(dimension_count < 16);
	_dimension_info.resize(dimension_count);
}

bool Yap::DimensionsImpl::SetDimensionInfo(unsigned int dimension_index, 
												  DimensionType dimension_type, 
												  unsigned int start_index, 
												  unsigned int length)
{
	assert(dimension_index < _dimension_info.size());
	_dimension_info[dimension_index].type = dimension_type; //type不变的应该用modify

	_dimension_info[dimension_index].start_index = start_index;
	_dimension_info[dimension_index].length = length;

	return true;
}

bool Yap::DimensionsImpl::ModifyDimension(DimensionType type, 
												 unsigned int length,
												 unsigned int start_index)
{
	for (unsigned int i = 0; i < _dimension_info.size(); ++i)
	{
		if (_dimension_info[i].type == type)
		{
			_dimension_info[i].length = length;
			_dimension_info[i].start_index = start_index;
			return true;
		}
	}

	return false;
}

unsigned int Yap::DimensionsImpl::GetLength(unsigned int dimension_index)
{
	assert(dimension_index < _dimension_info.size());

	return _dimension_info[dimension_index].length;
}

Yap::Dimension::Dimension(DimensionType type_, 
	unsigned start_index_, 
	unsigned int length_) :
	type(type_), start_index(start_index_), length(length_)
{
}

Yap::Dimension::Dimension() :
	start_index(0),
	length(0),
	type(DimensionInvalid)
{
}
