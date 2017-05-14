#include "ZeroFilling.h"

#include "Client/DataHelper.h"
#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"

#include <string>
#include <complex>

using namespace std;
using namespace Yap;

template <typename T>
bool zero_filling(T* dest, 
				 int dest_width, 
				 int dest_height,
				 T* source, 
				 int source_width, 
				 int source_height)
{
	assert(dest != nullptr && source != nullptr);
	assert(dest_width >= source_width && dest_height >= source_height);

	memset(dest, 0, dest_width * dest_height * sizeof(T));
	for (int row = 0; row < source_height; ++row)
	{
		memcpy(dest + ((dest_height - source_height) / 2 + row) * dest_width + (dest_width - source_width) / 2,
			source + row * source_width, source_width * sizeof(T));
	}

	return true;
}

ZeroFilling::ZeroFilling() : ProcessorImpl(L"ZeroFilling")
{
	LOG_TRACE(L"ZeroFilling constructor called.", L"BasicRecon");
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);

	AddProperty<int>(L"DestWidth", 256, L"Destination width.");
	AddProperty<int>(L"DestHeight", 256, L"Destination height.");
	AddProperty<int>(L"Left", 0, L"X coordinate of top left corner of source data in destination data.");
	AddProperty<int>(L"Top", 0, L"Y coordinate of top left corner of source data in destination data.");
}

ZeroFilling::ZeroFilling(const ZeroFilling& rhs)
	:ProcessorImpl(rhs)
{
	LOG_TRACE(L"ZeroFilling constructor called.", L"BasicRecon");
}

ZeroFilling::~ZeroFilling()
{
	LOG_TRACE(L"ZeroFilling destructor called.", L"BasicRecon");
}

bool ZeroFilling::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	unsigned int dest_width(GetProperty<int>(L"DestWidth"));
	unsigned int dest_height(GetProperty<int>(L"DestHeight"));

	DataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble && input_data.GetDataType() != DataTypeComplexFloat)
		return false;

	if (input_data.GetActualDimensionCount() != 2)
		return false;

	if (dest_width < input_data.GetWidth() || dest_height < input_data.GetHeight())
		return false;

	Yap::Dimensions dims;
	dims(DimensionReadout, 0, dest_width)
		(DimensionPhaseEncoding, 0, dest_height)
		(DimensionSlice, 0, 1)
		(Dimension4, 0, 1)
		(DimensionChannel, 0, 1);

	if (data->GetDataType() == DataTypeComplexDouble)
	{
		auto output = YapShared(new ComplexDoubleData(&dims));
		zero_filling(Yap::GetDataArray<complex<double>>(output.get()), dest_width, dest_height,
			Yap::GetDataArray<complex<double>>(data), input_data.GetWidth(), input_data.GetHeight());

		Feed(L"Output", output.get());
	}
	else
	{
		auto output = YapShared(new ComplexFloatData(&dims));
		zero_filling(Yap::GetDataArray<complex<float>>(output.get()), dest_width, dest_height,
					Yap::GetDataArray<complex<float>>(data), input_data.GetWidth(), input_data.GetHeight());

		Feed(L"Output", output.get());
	}
	
	return true;
}



