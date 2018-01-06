#include "stdafx.h"
#include "CalcuArea.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"

#include <numeric>


using namespace Yap;
CalcuArea::CalcuArea() :
	ProcessorImpl(L"CalcuArea")
{
	LOG_TRACE(L"CalcuArea constructor called.", L"BasicRecon");
	AddInput(L"Input", 1, DataTypeDouble);
	AddProperty<double>(L"Area", 0, L"Area of function or sum of data");
}

CalcuArea::CalcuArea(const CalcuArea& rhs) :
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"CalcuArea copy constructor called.", L"BasicRecon");
}

CalcuArea::~CalcuArea()
{
	LOG_TRACE(L"CalcuArea destructor called.", L"BasicRecon");
}

bool Yap::CalcuArea::Input(const wchar_t * name, IData * data)
{
	if (data == nullptr && _wcsicmp(name, L"Input") != 0)
		return false;
	if (data->GetDataType() != DataTypeDouble)
		return false;

	DataHelper input_data(data);
	if (input_data.GetActualDimensionCount() != 1)
		return false;

	unsigned int width = input_data.GetWidth();

	std::vector<double> temp_data(width, 0);
	std::memcpy(temp_data.data(), GetDataArray<double>(data), width * sizeof(double));
	double area = accumulate(temp_data.begin(), temp_data.end(), 0.0);
	SetProperty<double>(L"Area", area);

	return true;
}
