#include "ZeroFilling.h"

#include "Interface/Client/DataHelper.h"
#include "Interface/Implement/DataObject.h"

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

}

ZeroFilling::~ZeroFilling()
{
}

bool Yap::ZeroFilling::OnInit()
{
	AddInput(L"Input", 2, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexDouble | DataTypeComplexFloat);

	AddProperty(PropertyInt, L"DestWidth", L"Destination width.");
	SetInt(L"DestWidth", 512);
	AddProperty(PropertyInt, L"DestHeight", L"Destination height.");
	SetInt(L"DestHeight", 512);
	AddProperty(PropertyInt, L"Left", L"X coordinate of top left corner of source data in destination data.");
	SetInt(L"Left", 0);
	AddProperty(PropertyInt, L"Top", L"Y coordinate of top left corner of source data in destination data.");
	SetInt(L"Top", 0);

	return true;
}

bool ZeroFilling::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	unsigned int dest_width(GetInt(L"DestWidth"));
	unsigned int dest_height(GetInt(L"DestHeight"));

	DataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble && input_data.GetDataType() != DataTypeComplexFloat)
		return false;

	if (input_data.GetActualDimensionCount() != 2)
		return false;

	if (dest_width < input_data.GetWidth() || dest_height < input_data.GetHeight())
		return false;

	Yap::Dimensions dims;
	dims(DimensionReadout, 0, dest_width)
		(DimensionPhaseEncoding, 0, dest_height);

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

Yap::IProcessor * Yap::ZeroFilling::Clone()
{
	return new (std::nothrow) ZeroFilling(*this);
}



