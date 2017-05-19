#include "stdafx.h"
#include "DataTypeConvertor.h"
#include "Client\DataHelper.h"
#include "Implement\LogUserImpl.h"

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
Yap::SmartPtr<IData> DataTypeConvertor::GetConvertedData(IData* input_object)
{
	assert(input_object != nullptr);

	auto output_object = CreateData<OUT_TYPE>(input_object->GetDimensions());
	auto output = ::GetDataArray<OUT_TYPE>(output_object.get());

	DataHelper helper(input_object);
	auto input = ::GetDataArray<IN_TYPE>(input_object);

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
	LOG_TRACE(L"DataTypeConvertor constructor called.", L"BasicRecon");
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
	LOG_TRACE(L"DataTypeConvertor constructor called.", L"BasicRecon");
}

DataTypeConvertor::~DataTypeConvertor()
{
	LOG_TRACE(L"DataTypeConvertor destructor called.", L"BasicRecon");
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

map<int, wchar_t *> DataTypeConvertor::s_data_type_to_port = {
	{DataTypeBool, L"Bool"},
	{DataTypeUnsignedChar, L"Char"},
	{DataTypeShort, L"Short"},
	{DataTypeUnsignedShort, L"UnsignedShort"},
	{DataTypeInt, L"Int"},
	{DataTypeUnsignedInt,L"UnsignedInt"},
	{DataTypeFloat, L"Float"},
	{DataTypeDouble, L"Double"},
	{DataTypeComplexFloat, L"ComplexFloat"},
	{DataTypeComplexDouble, L"ComplexDouble"},
};

const wchar_t * DataTypeConvertor::GetPortName(int data_type)
{

	auto iter = s_data_type_to_port.find(data_type);

	return (iter != s_data_type_to_port.end()) ? iter->second : nullptr;
}

bool Yap::DataTypeConvertor::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	int output_data_type = GetOutputDataType();
	switch (data->GetDataType())
	{
	case DataTypeBool:
	{
		auto output = Convert<bool>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	case DataTypeUnsignedChar:
	{
		auto output = Convert<unsigned char>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	case DataTypeShort:
	{
		auto output = Convert<short>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	case DataTypeUnsignedShort:
	{
		auto output = Convert<unsigned short>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	case DataTypeFloat:
	{
		auto output = Convert<float>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	case DataTypeDouble:
	{
		auto output = Convert<double>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	case DataTypeInt:
	{
		auto output = Convert<int>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	case DataTypeUnsignedInt:
	{
		auto output = Convert<unsigned int>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	case DataTypeComplexFloat:
	{
		auto output = Convert<complex<float>>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	case DataTypeComplexDouble:
	{
		auto output = Convert<complex<double>>(data, output_data_type);
		return output ? Feed(GetPortName(output_data_type), output.get()) : false;
	}
	default:
		return false;
	}
}

int Yap::DataTypeConvertor::GetOutputDataType()
{
	for (auto item : s_data_type_to_port)
	{
		if (OutportLinked(item.second))
			return item.first;
	}

	return DataTypeUnknown;
}
