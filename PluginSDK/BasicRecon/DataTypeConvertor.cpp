#include "stdafx.h"
#include "DataTypeConvertor.h"
#include "Interface\Client\DataHelper.h"

using namespace Yap;
using namespace std;

template <typename IN_TYPE, typename OUT_TYPE>
Yap::SmartPtr<IData> GetConvertedData(IData* input_object)
{
	assert(input_object != nullptr);

	auto output_object = YapShared(new DataObject<OUT_TYPE>(input_object->GetDimensions()));
	auto output = ::GetDataArray<OUT_TYPE>(output_object.get());

	DataHelper helper(input_object);

	auto input = ::GetDataArray<IN_TYPE>(input_object);
	auto input_end = input + helper.GetDataSize();

	while (input != input_end)
	{
		*(output++) = OUT_TYPE(*(input++));
	}

	return output_object;
}

template <typename IN_TYPE>
Yap::SmartPtr<IData> GetConvertedDataComplexDouble(IData* input_object)
{
	assert(input_object != nullptr);

	auto output_object = YapShared(new DataObject<complex<double>>(input_object->GetDimensions()));
	auto output = ::GetDataArray<complex<double>>(output_object.get());

	DataHelper helper(input_object);

	auto input = ::GetDataArray<IN_TYPE>(input_object);
	auto input_end = input + helper.GetDataSize();

	while (input != input_end)
	{
		*(output++) = complex<double>(static_cast<double>(*(input++)));
	}

	return output_object;
}

template <typename IN_TYPE>
Yap::SmartPtr<IData> GetConvertedDataComplexFloat(IData* input_object)
{
	assert(input_object != nullptr);

	auto output_object = YapShared(new DataObject<complex<float>>(input_object->GetDimensions()));
	auto output = ::GetDataArray<complex<float>>(output_object.get());

	DataHelper helper(input_object);

	auto input = ::GetDataArray<IN_TYPE>(input_object);
	auto input_end = input + helper.GetDataSize();

	while (input != input_end)
	{
		*(output++) = complex<float>(static_cast<float>(*(input++)));
	}

	return output_object;
}

template <typename IN_TYPE>
Yap::SmartPtr<IData> Convert(IData * input, int output_type)
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
		return GetConvertedDataComplexFloat<IN_TYPE>(input);
	case DataTypeComplexDouble:
		return GetConvertedDataComplexDouble<IN_TYPE>(input);
	default:
		return SmartPtr<IData>();
	}
}

template <>
Yap::SmartPtr<IData> Convert<complex<double>>(IData * input, int output_type)
{
	assert(input != nullptr);

	DataHelper helper(input);
	auto data_size = helper.GetDataSize();

	switch (output_type)
	{
	case DataTypeComplexFloat:
		return GetConvertedData<complex<double>, complex<float>>(input);
	default:
		return SmartPtr<IData>();
	}
}

template <>
Yap::SmartPtr<IData> Convert<complex<float>>(IData * input, int output_type)
{
	assert(input != nullptr);

	DataHelper helper(input);
	auto data_size = helper.GetDataSize();

	switch (output_type)
	{
	case DataTypeComplexDouble:
		return GetConvertedData<complex<float>, complex<double>>(input);
	default:
		return SmartPtr<IData>();
	}
}

DataTypeConvertor::DataTypeConvertor(void):
	ProcessorImpl(L"DataTypeConvertor")
{
}

DataTypeConvertor::DataTypeConvertor(const DataTypeConvertor& rhs) :
	ProcessorImpl(rhs)
{

}

DataTypeConvertor::~DataTypeConvertor()
{
}

bool Yap::DataTypeConvertor::OnInit()
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeAll);

	AddOutput(L"Int", YAP_ANY_DIMENSION, DataTypeInt);
	AddOutput(L"UnsignedShort", YAP_ANY_DIMENSION, DataTypeUnsignedShort);
	AddOutput(L"Float", YAP_ANY_DIMENSION, DataTypeFloat);
	AddOutput(L"Double", YAP_ANY_DIMENSION, DataTypeDouble);
	AddOutput(L"ComplexFloat", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"ComplexDouble", YAP_ANY_DIMENSION, DataTypeComplexDouble);
// 	AddOutput(L"Bool", YAP_ANY_DIMENSION, DataTypeBool);
// 	AddOutput(L"Char", YAP_ANY_DIMENSION, DataTypeUnsignedChar);
// 	AddOutput(L"UnsignedInt", YAP_ANY_DIMENSION, DataTypeUnsignedInt);
// 	AddOutput(L"Short", YAP_ANY_DIMENSION, DataTypeShort);

	return true;
}

IProcessor * Yap::DataTypeConvertor::Clone()
{
	return new (nothrow) DataTypeConvertor(*this);
}

static const wchar_t * GetPortName(int data_type)
{
	static map<int, wchar_t *> data_type_mapping = {
		{DataTypeUnsignedChar, L"Char"},
		{DataTypeShort, L"Short"},
		{DataTypeUnsignedShort, L"UnsignedShort"},
		{DataTypeFloat, L"Float"},
		{DataTypeDouble, L"Double"},
		{DataTypeInt, L"Int"},
		{DataTypeUnsignedInt,L"UnsignedInt"},
		{DataTypeComplexFloat, L"ComplexFloat"},
		{DataTypeComplexDouble, L"ComplexDouble"},
		{DataTypeBool, L"Bool"} };

	auto iter = data_type_mapping.find(data_type);

	return (iter != data_type_mapping.end()) ? iter->second : nullptr;
}

bool Yap::DataTypeConvertor::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	int output_data_type = GetOutputDataType();
	switch (data->GetDataType())
	{
	case DataTypeUnsignedChar:
	{
		auto output = Convert<unsigned char>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}

	case DataTypeShort:
	{
		auto output = Convert<short>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}
	case DataTypeUnsignedShort:
	{
		auto output = Convert<unsigned short>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}
	case DataTypeFloat:
	{
		auto output = Convert<float>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}
	case DataTypeDouble:
	{
		auto output = Convert<double>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}
	case DataTypeInt:
	{
		auto output = Convert<int>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}
	case DataTypeUnsignedInt:
	{
		auto output = Convert<unsigned int>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}
	case DataTypeComplexFloat:
	{
		auto output = Convert<complex<float>>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}
	case DataTypeComplexDouble:
	{
		auto output = Convert<complex<double>>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}
	case DataTypeBool:
	{
		auto output = Convert<bool>(data, output_data_type);
		Feed(GetPortName(output_data_type), output.get());
		break;
	}
	default:
		return false;
	}

	return true;
}

int Yap::DataTypeConvertor::GetOutputDataType()
{
	int output_type;

	if (OutportLinked(L"Int"))
	{
		output_type = DataTypeInt;
	}
	else if (OutportLinked(L"UnsignedShort"))
	{
		output_type = DataTypeUnsignedShort;
	}
	else if (OutportLinked(L"Float"))
	{
		output_type = DataTypeFloat;
	}
	else if (OutportLinked(L"Double"))
	{
		output_type = DataTypeDouble;
	}
	else if (OutportLinked(L"ComplexFloat"))
	{
		output_type = DataTypeComplexFloat;
	}
	else if (OutportLinked(L"ComplexDouble"))
	{
		output_type = DataTypeComplexDouble;
	}

	return output_type;
}
