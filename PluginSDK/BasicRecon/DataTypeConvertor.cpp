#include "stdafx.h"
#include "DataTypeConvertor.h"
#include "Client\DataHelper.h"
#include "Implement\LogUserImpl.h"
#include <algorithm>

using namespace Yap;
using namespace std;

template <typename IN_TYPE, typename OUT_TYPE>
inline void ConvertElement(IN_TYPE& input, OUT_TYPE& output)
{
	output = OUT_TYPE(input);
}

template <typename IN_TYPE>
inline void ConvertElement(IN_TYPE& input, complex<double>& output)
{
	output = complex<double>(static_cast<double>(input));
}

template <typename IN_TYPE>
inline void ConvertElement(IN_TYPE& input, complex<float>& output)
{
	output = complex<float>(static_cast<float>(input));
}

inline void ConvertElement(complex<double>& input, complex<float>& output)
{
	output = complex<float>(input);
}

inline void ConvertElement(complex<float>& input, complex<double>& output)
{
	output = complex<double>(input);
}

template <typename IN_TYPE, typename OUT_TYPE>
Yap::SmartPtr<IData> DataTypeConvertor::GetConvertedData(IData* data)
{
	assert(data != nullptr);

	auto output_object = CreateData<OUT_TYPE>(data);
	auto output = ::GetDataArray<OUT_TYPE>(output_object.get());

	DataHelper helper(data);
	auto input = ::GetDataArray<IN_TYPE>(data);

	auto input_end = input + helper.GetDataSize();

	while (input != input_end)
	{
		ConvertElement(*(input++), *(output++));
	}

	return output_object;
}

template <typename IN_TYPE>
Yap::SmartPtr<IData> DataTypeConvertor::Convert(IData * input, int output_type)
{
	assert(input != nullptr);

	switch (output_type)
	{
	case DataTypeInt:
		return GetConvertedData<IN_TYPE, int>(input);
	case DataTypeUnsignedShort:
		return GetConvertedData<IN_TYPE, unsigned short>(input);
	case DataTypeFloat:
		return GetConvertedData<IN_TYPE, float>(input);
	case DataTypeDouble:
		return GetConvertedData<IN_TYPE, double>(input);
	case DataTypeComplexFloat:
		return GetConvertedData<IN_TYPE, complex<float>>(input);
	case DataTypeComplexDouble:
		return GetConvertedData<IN_TYPE, complex<double>>(input);
	default:
		return SmartPtr<IData>();
	}
}

template <>
Yap::SmartPtr<IData> DataTypeConvertor::Convert<complex<double>>(IData * input, int output_type)
{
	assert(input != nullptr);

	DataHelper helper(input);
	auto data_size = helper.GetDataSize();

	return (output_type == DataTypeComplexFloat) ?
		GetConvertedData<complex<double>, complex<float>>(input) :
		SmartPtr<IData>();
}

template <>
Yap::SmartPtr<IData> DataTypeConvertor::Convert<complex<float>>(IData * input, int output_type)
{
	assert(input != nullptr);

	DataHelper helper(input);
	auto data_size = helper.GetDataSize();

	return (output_type == DataTypeComplexDouble) ?
		GetConvertedData<complex<float>, complex<double>>(input) :
		SmartPtr<IData>();
}

DataTypeConvertor::DataTypeConvertor(void):
	ProcessorImpl(L"DataTypeConvertor")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeAll);

	AddOutput(L"Bool", YAP_ANY_DIMENSION, DataTypeBool);
	AddOutput(L"Char", YAP_ANY_DIMENSION, DataTypeUnsignedChar);
	AddOutput(L"Short", YAP_ANY_DIMENSION, DataTypeShort);
	AddOutput(L"UnsignedShort", YAP_ANY_DIMENSION, DataTypeUnsignedShort);
	AddOutput(L"Int", YAP_ANY_DIMENSION, DataTypeInt);
	AddOutput(L"UnsignedInt", YAP_ANY_DIMENSION, DataTypeUnsignedInt);
	AddOutput(L"Float", YAP_ANY_DIMENSION, DataTypeFloat);
	AddOutput(L"Double", YAP_ANY_DIMENSION, DataTypeDouble);
	AddOutput(L"ComplexFloat", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"ComplexDouble", YAP_ANY_DIMENSION, DataTypeComplexDouble);
}

DataTypeConvertor::DataTypeConvertor(const DataTypeConvertor& rhs) :
	ProcessorImpl(rhs)
{
}

DataTypeConvertor::~DataTypeConvertor()
{
}

static int DataTypeFromPortName(const wchar_t * port_name)
{
	static map<wstring, int> data_type_from_port_name = {
		{L"Bool", DataTypeBool},
		{L"Char", DataTypeUnsignedChar},
		{L"Short", DataTypeShort},
		{L"UnsignedShort", DataTypeUnsignedShort},
		{L"Int", DataTypeInt},
		{L"UnsignedInt", DataTypeUnsignedInt},
		{L"Float", DataTypeFloat},
		{L"Double", DataTypeDouble},
		{L"ComplexFloat", DataTypeComplexFloat},
		{L"ComplexDouble", DataTypeComplexDouble},
	};

	auto iter = data_type_from_port_name.find(port_name);
	return iter != data_type_from_port_name.end() ?
		iter->second : DataTypeUnknown;
}

template <typename IN_TYPE>
bool Yap::DataTypeConvertor::ConvertAndFeed(IData * input_data)
{
	bool success = true;
	for (auto link : _links)
	{
		auto output = Convert<IN_TYPE>(input_data, DataTypeFromPortName(link.first.c_str()));
		if (!output || !Feed(link.first.c_str(), output.get()))
		{
			success = false;
		}
	};
	return success;
}

bool Yap::DataTypeConvertor::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	bool success = true;
	switch (data->GetDataType())
	{
	case DataTypeBool:
		return ConvertAndFeed<bool>(data);
	case DataTypeUnsignedChar:
		return ConvertAndFeed<unsigned char>(data);
	case DataTypeShort:
		return ConvertAndFeed<short>(data);
	case DataTypeUnsignedShort:
		return ConvertAndFeed<unsigned short>(data);
	case DataTypeFloat:
		return ConvertAndFeed<float>(data);
	case DataTypeDouble:
		return ConvertAndFeed<double>(data);
	case DataTypeInt:
		return ConvertAndFeed<int>(data);
	case DataTypeUnsignedInt:
		return ConvertAndFeed<unsigned int>(data);
	case DataTypeComplexFloat:
		return ConvertAndFeed<complex<float>>(data);
	case DataTypeComplexDouble:
		return ConvertAndFeed<complex<double>>(data);
	default:
		return false;
	}
}

