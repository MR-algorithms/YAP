#include "DataImpl.h"

using namespace Yap;

/**
	����������ʹ�õ��ܵ�ά����
*/
unsigned int Yap::CDimensionsImpl::GetDimensionCount() 
{
	return static_cast<unsigned int>(_dimension_info.size());
}

/**
	���ָ����ά�ȵ���Ϣ��
	@param  dimension_index ָ��ϣ���������Ϣ��ά�ȵ���������0��ʼ��
	@param  dimension_type ���������ָ�����������ͣ����壩��
	@param  start_index ��ص����������ά���ϵ���ʼλ�á�
	@param  length ��ص����������ά���ϵĿ�ȡ�
	@return
*/
bool Yap::CDimensionsImpl::GetDimensionInfo(unsigned int dimension_index, 
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

IClonable * Yap::CDimensionsImpl::Clone()
{
	return new (std::nothrow) CDimensionsImpl(*this);
}

CDimensionsImpl & Yap::CDimensionsImpl::operator()(DimensionType type, unsigned int index, unsigned int length)
{
	assert(type != DimensionInvalid);

	_dimension_info.push_back(Dimension(type, index, length));

	return *this;
}

Yap::CDimensionsImpl::CDimensionsImpl()
{
}

Yap::CDimensionsImpl::CDimensionsImpl(const CDimensionsImpl& source)
{
	_dimension_info = source._dimension_info;
}

Yap::CDimensionsImpl::CDimensionsImpl(IDimensions * source)
{
	DimensionType type;
	unsigned int index, length;

	for (unsigned int i = 0; i < source->GetDimensionCount(); ++i)
	{
		source->GetDimensionInfo(i, type, index, length);
		_dimension_info.push_back(Dimension(type, index, length));
	}
}

Yap::CDimensionsImpl::CDimensionsImpl(unsigned int dimension_count)
{
	assert(dimension_count < 16);
	_dimension_info.resize(dimension_count);
}

bool Yap::CDimensionsImpl::SetDimensionInfo(unsigned int dimension_index, 
												  DimensionType dimension_type, 
												  unsigned int start_index, 
												  unsigned int length)
{
	assert(dimension_index < _dimension_info.size());
	_dimension_info[dimension_index].type = dimension_type; //type�����Ӧ����modify

	_dimension_info[dimension_index].start_index = start_index;
	_dimension_info[dimension_index].length = length;

	return true;
}

bool Yap::CDimensionsImpl::ModifyDimension(DimensionType type, 
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

unsigned int Yap::CDimensionsImpl::GetLength(unsigned int dimension_index)
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
