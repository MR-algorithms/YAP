#include "stdafx.h"
#include "ComplexSplitter.h"

using namespace std;
using namespace Yap;

CComplexSplitter::CComplexSplitter() :
	CProcessorImp(L"ComplexSplitter")
{
	// ANY  DIMENSION
	AddInputPort(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble);
	AddOutputPort(L"Real", YAP_ANY_DIMENSION, DataTypeDouble);
	AddOutputPort(L"Imaginary", YAP_ANY_DIMENSION, DataTypeDouble);
}

CComplexSplitter::~CComplexSplitter()
{
}

bool CComplexSplitter::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input" )
		return false;
	
	if (data->GetDataType() != DataTypeComplexDouble)
		return false;

	CDataHelper input_data(data);

	auto want_real = OutportLinked(L"Real");
	auto want_imaginary = OutportLinked(L"Imaginary");

	//one, two and three dimension(s), ANY¡¡DIMENSION
	auto size = static_cast<unsigned int> (input_data.GetDataSize());

	if (want_real && want_imaginary)
	{
		auto * real_data = new Yap::CDoubleData(data->GetDimensions());
		auto * imaginary_data = new Yap::CDoubleData(data->GetDimensions());
		Split(reinterpret_cast<std::complex<double> *>(input_data.GetData()),
			reinterpret_cast<double*>(real_data->GetData()),
			reinterpret_cast<double*>(imaginary_data->GetData()),
			size);
		Feed(L"Real", real_data);
		Feed(L"Imaginary", imaginary_data);
	}
	else if (want_real)
	{
		auto * real_data = new Yap::CDoubleData(data->GetDimensions());
		ExtractReal(reinterpret_cast<std::complex<double> *>(input_data.GetData()),
			reinterpret_cast<double*>(real_data->GetData()),
			size);
		Feed(L"Real", real_data);
	}
	else if (want_imaginary)
	{
		auto * imaginary_data = new Yap::CDoubleData(data->GetDimensions());
		ExtractImaginary(reinterpret_cast<std::complex<double> *>(input_data.GetData()),
			reinterpret_cast<double*>(imaginary_data->GetData()),
			size);
		Feed(L"Imaginary", imaginary_data);
	}

	return true;
}

void Yap::CComplexSplitter::Split(std::complex<double> * data, 
	double * real, 
	double * imaginary, size_t size)
{
	assert(data != nullptr && real != nullptr && imaginary != nullptr);

	for (auto cursor = reinterpret_cast<double*>(data); cursor != reinterpret_cast<double*>(data + size); )
	{
		*real++ = *cursor++;
		*imaginary++ = *cursor++;
	}
}

void CComplexSplitter::ExtractReal(std::complex<double> * data, double * real, size_t size)
{
	assert(data != nullptr && real != nullptr);

	for (double * cursor = reinterpret_cast<double*>(data); cursor != reinterpret_cast<double*>(data + size); cursor += 2)
	{
		*real++ = *cursor;
	}
}

void CComplexSplitter::ExtractImaginary(std::complex<double> * data, double * imaginary, size_t size)
{
	assert(data != nullptr && imaginary != nullptr);

	for (double * cursor = reinterpret_cast<double*>(data) + 1; cursor != reinterpret_cast<double*>(data + size) + 1; cursor += 2)
	{
		*imaginary++ = *cursor;
	}
}
