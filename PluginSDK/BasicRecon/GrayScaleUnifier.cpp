#include "stdafx.h"
#include "GrayScaleUnifier.h"
#include "Implement/LogUserImpl.h"
#include "Client/DataHelper.h"

#include <algorithm>

using namespace Yap;
using namespace std;

Yap::GrayScaleUnifier::GrayScaleUnifier():
	ProcessorImpl(L"GrayScaleUnifier")
{
	LOG_TRACE(L"GrayScaleUnifier constructor called.", L"BasicRecon");
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeFloat);
}

Yap::GrayScaleUnifier::GrayScaleUnifier(const GrayScaleUnifier& rhs) :
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"GrayScaleUnifier constructor called.", L"BasicRecon");
}

Yap::GrayScaleUnifier::~GrayScaleUnifier()
{
	LOG_TRACE(L"GrayScaleUnifier destructor called.", L"BasicRecon");
}

bool Yap::GrayScaleUnifier::Input(const wchar_t * name, IData * data)
{
	if (wstring(name) != L"Input")
		return false;

	DataHelper input(data);

	if (input.GetDataType() != DataTypeFloat)
		return false;

	auto size = input.GetDataSize();
	vector<float> temp_data(size, 0);
	memcpy(temp_data.data(), GetDataArray<float>(data), size * sizeof(float));
	auto max_val = *max_element(temp_data.begin(), temp_data.end());
	auto min_val = *min_element(temp_data.begin(), temp_data.end());

	auto output = CreateData<float>(data);
	auto input_data = GetDataArray<float>(data);
	auto output_data = GetDataArray<float>(output.get());
	auto input_end = input_data + size;

	auto rate = 255.0 / (max_val - min_val);
	while (input_data != input_end)
	{
		*(output_data++) = static_cast<float>((*(input_data++) - min_val) * rate);
	}

	return Feed(L"Output", output.get());
}
