#include "stdafx.h"
#include "DataTypeConvertor.h"
#include "Interface\Client\DataHelper.h"

using namespace Yap;
using namespace std;

template <typename T>
bool ReverseType(T* input,
	T* target_type_data,
	size_t size)
{
	assert(input != nullptr && target_type_data != nullptr);

	auto input_end = input + size;
	while (input != input_end)
	{
		*(target_type_data++) = *(input++);
	}

	return true;
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
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeFloat);//DataType´ý¸Ä

	AddOutput(L"Int", YAP_ANY_DIMENSION, DataTypeInt);
	AddOutput(L"UnsignedShort", YAP_ANY_DIMENSION, DataTypeUnsignedShort);
	AddOutput(L"Float", YAP_ANY_DIMENSION, DataTypeFloat);
	AddOutput(L"Double", YAP_ANY_DIMENSION, DataTypeDouble);
	AddOutput(L"ComplexFloat", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"ComplexDouble", YAP_ANY_DIMENSION, DataTypeComplexDouble);

	return true;
}

IProcessor * Yap::DataTypeConvertor::Clone()
{
	return new (nothrow) DataTypeConvertor(*this);
}

bool Yap::DataTypeConvertor::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	DataHelper input_data(data);

	auto want_int = OutportLinked(L"Int");
	auto want_unsignedshort = OutportLinked(L"UnsignedShort");
	auto want_float = OutportLinked(L"Float");
	auto want_double = OutportLinked(L"Double");
	auto want_complexFloat = OutportLinked(L"ComplexFloat");
	auto want_complexDouble = OutportLinked(L"ComplexDouble");

	if (input_data.GetActualDimensionCount() != 2)
		return false;

	Dimensions dims;
	dims(DimensionReadout, 0, input_data.GetWidth())
		(DimensionPhaseEncoding, 0, input_data.GetHeight());

	if (want_int)
	{
		auto int_data = YapShared(new IntData(&dims));

		ReverseType(GetDataArray<int>(data),
			GetDataArray<int>(int_data.get()),
			input_data.GetDataSize());

		Feed(L"Int", int_data.get());
	}

	if (want_unsignedshort)
	{
		auto unsignedshort_data = YapShared(new UnsignedShortData(&dims));

		auto input = GetDataArray<float>(data);
		auto taget = GetDataArray<unsigned short>(unsignedshort_data.get());
		auto size = input_data.GetDataSize();

		assert(input != nullptr && taget != nullptr);

		auto input_end = input + size;
		while (input != input_end)
		{
			*(taget++) = *((unsigned short *)input++);
		}

// 		ReverseType(GetDataArray<unsigned short>(data),
// 			GetDataArray<unsigned short>(unsignedshort_data.get()),
// 			input_data.GetDataSize());

		Feed(L"UnsignedShort", unsignedshort_data.get());
	}

	if (want_float)
	{
		auto float_data = YapShared(new FloatData(&dims));

		ReverseType(GetDataArray<float>(data),
			GetDataArray<float>(float_data.get()),
			input_data.GetDataSize());

		Feed(L"Float", float_data.get());
	}

	if (want_double)
	{
		auto double_data = YapShared(new DoubleData(&dims));

		ReverseType(GetDataArray<double>(data),
			GetDataArray<double>(double_data.get()),
			input_data.GetDataSize());

		Feed(L"Double", double_data.get());
	}

	if (want_complexFloat)
	{
		auto complexfloat_data = YapShared(new ComplexFloatData(&dims));

		ReverseType(GetDataArray<complex<float>>(data),
			GetDataArray<complex<float>>(complexfloat_data.get()),
			input_data.GetDataSize());

		Feed(L"ComplexFloat", complexfloat_data.get());
	}

	if (want_complexDouble)
	{
		auto complexdouble_data = YapShared(new ComplexDoubleData(&dims));

		ReverseType(GetDataArray<complex<double>>(data),
			GetDataArray<complex<double>>(complexdouble_data.get()),
			input_data.GetDataSize());

		Feed(L"ComplexDouble", complexdouble_data.get());
	}

	return true;
}