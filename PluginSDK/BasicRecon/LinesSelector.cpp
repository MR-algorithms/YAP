#include "stdafx.h"
#include "LinesSelector.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"

using namespace std;
using namespace Yap;

Yap::LinesSelector::LinesSelector():
	ProcessorImpl(L"LinesSelector")
{
	LOG_TRACE(L"LinesSelector constructor called.", L"BasicRecon");
	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);

	AddProperty<int>(L"FirstLineIndex", 0, L"The index of the first line you want to get.");
	AddProperty<int>(L"LinesCount", 1, L"The count of lines you want to get.");
}

Yap::LinesSelector::LinesSelector(const LinesSelector& rhs):
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"LinesSelector constructor called.", L"BasicRecon");
}

Yap::LinesSelector::~LinesSelector()
{
	LOG_TRACE(L"LinesSelector destructor called.", L"BasicRecon");
}

bool Yap::LinesSelector::Input(const wchar_t * name, IData * data)
{
	// Do some check.
	if (data == nullptr)
	{
		LOG_ERROR(L"<LineSelector> Invalid input data!", L"BasicRecon");
		return false;
	}
	if (_wcsicmp(name, L"Input") != 0)
	{
		LOG_ERROR(L"<LineSelector> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper input_data(data);
	if (input_data.GetActualDimensionCount() != 2)
	{
		LOG_ERROR(L"<LineSelector> Error input data dimention!(2D data is available)!", L"BasicRecon");
		return false;
	}
	if (input_data.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<LineSelector> Error input data type!(DataTypeComplexFloat are available)!", L"BasicRecon");
		return false;
	}

	int first_line_index = GetProperty<int>(L"FirstLineIndex");
	int lines_count = GetProperty<int>(L"LinesCount");

	if (unsigned int(first_line_index + lines_count) > input_data.GetHeight() ||
		first_line_index < 0 || lines_count <= 0)
	{
		LOG_ERROR(L"<LineSelector> Improper properties(FirstLineIndex, LineCount) value.", L"BasicRecon");
		return false;
	}

	Yap::Dimensions dims;
	dims(DimensionReadout, 0, input_data.GetWidth())
		(DimensionPhaseEncoding, 0, lines_count);
	auto output = CreateData<complex<float>>(data, &dims);

	memset(Yap::GetDataArray<complex<float>>(output.get()), 0,
		input_data.GetWidth() * lines_count * sizeof(complex<float>));

	memcpy(Yap::GetDataArray<complex<float>>(output.get()),
		Yap::GetDataArray<complex<float>>(data) + first_line_index * input_data.GetWidth(),
		input_data.GetWidth() * lines_count * sizeof(complex<float>));

	return Feed(L"Output", output.get());
}