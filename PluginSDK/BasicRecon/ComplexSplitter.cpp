#include "ComplexSplitter.h"
#include "Implement/LogUserImpl.h"

using namespace std;
using namespace Yap;

ComplexSplitter::ComplexSplitter() :
	ProcessorImpl(L"ComplexSplitter")
{
	LOG_TRACE(L"ComplexSplitter constructor called.", L"BasicRecon");
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble);
	AddOutput(L"Real", YAP_ANY_DIMENSION, DataTypeDouble);
	AddOutput(L"Imaginary", YAP_ANY_DIMENSION, DataTypeDouble);
}

ComplexSplitter::~ComplexSplitter()
{
}

bool ComplexSplitter::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input" )
		return false;
	
	if (data->GetDataType() != DataTypeComplexDouble)
		return false;

	DataHelper input_data(data);

	auto want_real = OutportLinked(L"Real");
	auto want_imaginary = OutportLinked(L"Imaginary");

	//one, two and three dimension(s), ANY　DIMENSION
	auto size = static_cast<unsigned int> (input_data.GetDataSize());
	auto data_array = GetDataArray<complex<double>>(data);
	if (want_real && want_imaginary)
	{
		auto real_data = CreateData<double>(data) ;
		auto imaginary_data = CreateData<double>(data);

		Split(data_array, GetDataArray<double>(real_data.get()), GetDataArray<double>(imaginary_data.get()), size);

		Feed(L"Real", real_data.get());
		Feed(L"Imaginary", imaginary_data.get());
	}
	else if (want_real)
	{
		auto real_data = CreateData<double>(data);
		ExtractReal(data_array, GetDataArray<double>(real_data.get()), size);
		Feed(L"Real", real_data.get());
	}
	else if (want_imaginary)
	{
		auto imaginary_data = CreateData<double>(data);
		ExtractImaginary(data_array, GetDataArray<double>(imaginary_data.get()), size);
		Feed(L"Imaginary", imaginary_data.get());
	}

	return true;
}

void Yap::ComplexSplitter::Split(std::complex<double> * data, 
	double * real, 
	double * imaginary, size_t size)
{
	assert(data != nullptr && real != nullptr && imaginary != nullptr);
	BUG(Review the pointer algorithms in this file.);

	for (auto cursor = reinterpret_cast<double*>(data); cursor != reinterpret_cast<double*>(data) + size; )
	{
		*real++ = *cursor++;
		*imaginary++ = *cursor++;
	}
}

Yap::ComplexSplitter::ComplexSplitter(const ComplexSplitter& rhs)
	:ProcessorImpl(rhs)
{
	LOG_TRACE(L"ComplexSplitter constructor called.", L"BasicRecon");
}

void ComplexSplitter::ExtractReal(std::complex<double> * data, double * real, size_t size)
{
	assert(data != nullptr && real != nullptr);

	for (double * cursor = reinterpret_cast<double*>(data); cursor != reinterpret_cast<double*>(data + size); cursor += 2)
	{
		*real++ = *cursor;
	}
}

void ComplexSplitter::ExtractImaginary(std::complex<double> * data, double * imaginary, size_t size)
{
	assert(data != nullptr && imaginary != nullptr);

	for (double * cursor = reinterpret_cast<double*>(data) + 1; cursor != reinterpret_cast<double*>(data) + size + 1; cursor += 2)
	{
		*imaginary++ = *cursor;
	}
}
