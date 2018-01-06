#include "stdafx.h"
#include "FineCF.h"
#include "Client\DataHelper.h"
#include "Implement\LogUserImpl.h"
#include <algorithm>
#include <vector>
#include <cassert>

using namespace std;
using namespace Yap;

FineCF::FineCF()
	: ProcessorImpl(L"FineCF")
{
	LOG_TRACE(L"FineCF constructor called.", L"BasicRecon");
	AddInput(L"Input", 1, DataTypeDouble | DataTypeFloat);
	AddProperty<double>(L"O1", 0, L"中心频率");
	AddProperty<double>(L"SW", 0, L"谱宽");
	AddProperty<bool>(L"ReverseAxis", false, L"负轴");
	AddProperty<double>(L"CF", 0, L"最高峰频率");
}

Yap::FineCF::FineCF(const FineCF& rhs) :
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"FineCF copy constructor called.", L"BasicRecon");
}


FineCF::~FineCF()
{
	LOG_TRACE(L"FineCF destructor called.", L"BasicRecon");
}

bool FineCF::Input(const wchar_t * port, IData * data)
{
	if (data == nullptr && _wcsicmp(port, L"Input") != 0)
		return false;

	if (data->GetDataType() != DataTypeDouble && data->GetDataType() != DataTypeFloat)
		return false;

	DataHelper input_data(data);
	if (input_data.GetActualDimensionCount() != 1)
		return false;

	unsigned int width = input_data.GetWidth();
	auto o1 = GetProperty<double>(L"O1");
	auto sw = GetProperty<double>(L"SW");
	int max_val_pos = 0;
	if (data->GetDataType() == DataTypeDouble)
	{
		std::vector<double> temp_data(width, 0);
		std::memcpy(temp_data.data(), GetDataArray<double>(data), width * sizeof(double));
		max_val_pos = std::max_element(temp_data.begin(), temp_data.end()) - temp_data.begin();
	}
	else
	{
		assert(data->GetDataType() == DataTypeFloat);
		std::vector<float> temp_data(width, 0);
		std::memcpy(temp_data.data(), GetDataArray<float>(data), width * sizeof(float));
		max_val_pos = std::max_element(temp_data.begin(), temp_data.end()) - temp_data.begin();
	}

	double offset = (max_val_pos - (int)width / 2) * sw / width;
	SetProperty<double>(L"CF", GetProperty<bool>(L"ReverseAxis") ? o1 - offset : o1 + offset);
	return true;
}
