#pragma once
#include "..\Interface\Interface.h"

class CData
{
public:
	CData(IData * data) : _data_interface(*data) {}

	DataType GetDataType();

	unsigned int GetDimensionCount();

	unsigned int GetWidth();

	unsigned int GetHeight();
	void * GetData();

protected:
	IData & _data_interface;
};
