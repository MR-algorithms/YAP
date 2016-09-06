#include "ComplexSplitter.h"

using namespace std;
using namespace Yap;

ComplexSplitter::ComplexSplitter() :
	ProcessorImpl(L"ComplexSplitter")
{
}

ComplexSplitter::~ComplexSplitter()
{
}


bool Yap::ComplexSplitter::OnInit()
{
	// ANY  DIMENSION
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble);
	AddOutput(L"Real", YAP_ANY_DIMENSION, DataTypeDouble);
	AddOutput(L"Imaginary", YAP_ANY_DIMENSION, DataTypeDouble);

	return true;
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

	//one, two and three dimension(s), ANY¡¡DIMENSION
	auto size = static_cast<unsigned int> (input_data.GetDataSize());
	auto data_array = GetDataArray<complex<double>>(data);
	if (want_real && want_imaginary)
	{
		auto * real_data = new Yap::DoubleData(data->GetDimensions());
		auto * imaginary_data = new Yap::DoubleData(data->GetDimensions());
		Split(data_array, GetDataArray<double>(real_data), GetDataArray<double>(imaginary_data), size);

		Feed(L"Real", real_data);
		Feed(L"Imaginary", imaginary_data);
	}
	else if (want_real)
	{
		auto * real_data = new Yap::DoubleData(data->GetDimensions());
		ExtractReal(data_array, GetDataArray<double>(real_data), size);
		Feed(L"Real", real_data);
	}
	else if (want_imaginary)
	{
		auto * imaginary_data = new Yap::DoubleData(data->GetDimensions());
		ExtractImaginary(data_array, GetDataArray<double>(imaginary_data), size);
		Feed(L"Imaginary", imaginary_data);
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

Yap::IProcessor * Yap::ComplexSplitter::Clone()
{
	return new(nothrow) ComplexSplitter(*this);
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
