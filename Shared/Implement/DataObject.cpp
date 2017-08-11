#include "DataObject.h"

using namespace Yap;

/**
	获得相关数据使用的总的维数。
*/
unsigned int Yap::Dimensions::GetDimensionCount() 
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
bool Yap::Dimensions::GetDimensionInfo(unsigned int dimension_index, 
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

Dimensions & Yap::Dimensions::operator()(DimensionType type, unsigned int index, unsigned int length)
{
	assert(type != DimensionInvalid);

	_dimension_info.push_back(Dimension(type, index, length));

	return *this;
}

Yap::Dimensions::Dimensions()
{
}

Yap::Dimensions::~Dimensions()
{

}

Yap::Dimensions::Dimensions(const Dimensions& source)
{
	_dimension_info = source._dimension_info;
}

Yap::Dimensions::Dimensions(IDimensions * source)
{
	DimensionType type = DimensionInvalid;
	unsigned int index = 0, length = 0;

	for (unsigned int i = 0; i < source->GetDimensionCount(); ++i)
	{
		source->GetDimensionInfo(i, type, index, length);
		_dimension_info.push_back(Dimension(type, index, length));
	}
}

Yap::Dimensions::Dimensions(unsigned int dimension_count)
{
	assert(dimension_count < 16);
	_dimension_info.resize(dimension_count);
}

bool Yap::Dimensions::SetDimensionInfo(unsigned int dimension_index, 
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

bool Yap::Dimensions::SetDimension(DimensionType type, 
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

unsigned int Yap::Dimensions::GetLength(unsigned int dimension_index)
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

void Yap::Localization::GetSpacing(double& x, double& y, double& z)
{
	x = _spacing_x;
	y = _spacing_y;
	z = _spacing_z;
}

void Yap::Localization::GetRowVector(double& x, double& y, double& z)
{
	x = _row.x;
	y = _row.y;
	z = _row.z;
}

void Yap::Localization::GetColumnVector(double& x, double& y, double& z)
{
	x = _column.x;
	y = _column.y;
	z = _column.z;
}

void Yap::Localization::GetSliceVector(double& x, double& y, double& z)
{
	x = _slice.x;
	y = _slice.y;
	z = _slice.z;
}

void Yap::Localization::GetReadoutVector(double& x, double& y, double& z)
{
	x = _readout.x;
	y = _readout.y;
	z = _readout.z;
}

void Yap::Localization::GetPhaseEncodingVector(double& x, double& y, double& z)
{
	x = _phase_encoding.x;
	y = _phase_encoding.y;
	z = _phase_encoding.z;
}

void Yap::Localization::GetCenter(double& x, double& y, double& z)
{
	x = _center.x;
	y = _center.y;
	z = _center.z;
}

void Yap::Localization::SetSpacing(double x, double y, double z)
{
	_spacing_x = x;
	_spacing_y = y;
	_spacing_z = z;
}

void Yap::Localization::SetRowVector(double x, double y, double z)
{
	_row.x = x;
	_row.y = y;
	_row.z = z;
}

void Yap::Localization::SetColumnVector(double x, double y, double z)
{
	_column.x = x;
	_column.y = y;
	_column.z = z;
}

void Yap::Localization::SetSliceVector(double x, double y, double z)
{
	_slice.x = x;
	_slice.y = y;
	_slice.z = z;
}

void Yap::Localization::SetReadoutVector(double x, double y, double z)
{
	_readout.x = x;
	_readout.y = y;
	_readout.z = z;
}

void Yap::Localization::SetPhaseEncodingVector(double x, double y, double z)
{
	_phase_encoding.x = x;
	_phase_encoding.y = y;
	_phase_encoding.z = z;
}

void Yap::Localization::SetCenter(double x, double y, double z)
{
	_center.x = x;
	_center.y = y;
	_center.z = z;
}

bool Yap::Localization::IsValid()
{
	throw std::logic_error("The method or operation is not implemented.");
}

Yap::Localization::Localization() :
	_spacing_x{ 0 },
	_spacing_y{ 0 },
	_spacing_z{ 0 },
	_row{ Vector() },
	_column{ Vector() },
	_slice{ Vector() },
	_readout{ Vector() },
	_phase_encoding{ Vector() },
	_center{ Point() }
{
}
	

Yap::Localization::Localization(const Localization& source) :
	_spacing_x{ source._spacing_x },
	_spacing_y{ source._spacing_y },
	_spacing_z{ source._spacing_z },
	_row{ source._row },
	_column{ source._column },
	_slice{ source._slice },
	_readout{ source._readout },
	_phase_encoding{ source._phase_encoding },
	_center{ source._center }
{

}

Yap::Localization::Localization(IGeometry * source)
{
	double x, y, z;
	if (source != nullptr)
	{
		source->GetSpacing(_spacing_x, _spacing_y, _spacing_z);
		source->GetRowVector(x, y, z);
		_row = Vector(x, y, z);
		source->GetColumnVector(x, y, z);
		_column = Vector(x, y, z);
		source->GetSliceVector(x, y, z);
		_slice = Vector(x, y, z);
		source->GetReadoutVector(x, y, z);
		_readout = Vector(x, y, z);
		source->GetPhaseEncodingVector(x, y, z);
		_phase_encoding = Vector(x, y, z);
		source->GetCenter(x, y, z);
		_center = Point(x, y, z);
	}
}

Yap::Localization::~Localization()
{

}