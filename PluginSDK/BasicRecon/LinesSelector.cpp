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
	if (wstring(name) != L"Input")
		return false;

	DataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexFloat)
		return false;

	if (input_data.GetActualDimensionCount() != 2)
		return false;

	int first_line_index = GetProperty<int>(L"FirstLineIndex");
	int lines_count = GetProperty<int>(L"LinesCount");

	if (first_line_index + lines_count > input_data.GetHeight() ||
		first_line_index < 0 || 
		lines_count <= 0)
		return false;

	Yap::Dimensions dims;
	dims(DimensionReadout, 0, input_data.GetWidth())
		(DimensionPhaseEncoding, 0, lines_count);
	auto output = CreateData<std::complex<float>>(data, &dims);

	memset(Yap::GetDataArray<complex<float>>(output.get()), 0,
		input_data.GetWidth() * lines_count * sizeof(complex<float>));

	memcpy(Yap::GetDataArray<complex<float>>(output.get()),
		Yap::GetDataArray<complex<float>>(data) + first_line_index * input_data.GetWidth(),
		input_data.GetWidth() * lines_count * sizeof(complex<float>));

	return Feed(L"Output", output.get());
}