#pragma once
#include "..\Interface\Interface.h"

class CDataHelper
{
public:
	CDataHelper(IData * data) : _data_interface(*data) {}

	DataType GetDataType();
	unsigned int GetDimensionCount();

	unsigned int GetWidth();

	unsigned int GetHeight();

	unsigned int GetCoilCount();

	void * GetData();

protected:
	IData & _data_interface;
};
