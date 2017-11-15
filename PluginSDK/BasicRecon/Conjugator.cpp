#include "stdafx.h"
#include "Conjugator.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"

using namespace Yap;
using namespace std; 

Yap::Conjugator::Conjugator():
	ProcessorImpl(L"Conjugator")
{
	LOG_TRACE(L"Conjugator constructor called.", L"BasicRecon");
	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);

	AddProperty<int>(L"FirstPointIndex", 0, L"The index of the first point you want to get.");
	AddProperty<int>(L"PointsCount", 0, L"The count of points you want to get.");
}

Yap::Conjugator::Conjugator(const Conjugator& rhs):
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"Conjugator constructor called.", L"BasicRecon");
}

Yap::Conjugator::~Conjugator()
{
	LOG_TRACE(L"Conjugator destructor called.", L"BasicRecon");
}

bool Yap::Conjugator::Input(const wchar_t * name, IData * data)
{
	if (wstring(name) != L"Input")
		return false;

	DataHelper input(data);
	if (input.GetDataType() != DataTypeComplexFloat)
		return false;

	if (input.GetActualDimensionCount() != 2)
		return false;

	auto size = GetProperty<int>(L"PointsCount");
	auto first_point_index = GetProperty<int>(L"FirstPointIndex");
	auto data_size = input.GetWidth() * input.GetHeight();
	if (size == 0)
	{
		size = data_size;
		first_point_index = 0;
	}

	auto output = CreateData<complex<float>>(data);
	auto input_data = GetDataArray<complex<float>>(data);
	auto output_data = GetDataArray<complex<float>>(output.get());

	if (input_data == nullptr || output_data == nullptr)
		return false;

	for (unsigned int i = 0; i < data_size; ++i)
	{
		if (i >= first_point_index && i < first_point_index + size)
		{
			*(output_data + i) = conj(*(input_data + (data_size - 1 - i)));
		}
		else
		{
			*(output_data + i) = *(input_data + i);
		}
	}

	return Feed(L"Output", output.get());
} 