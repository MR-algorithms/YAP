#include "ZeroFilling.h"

#include "Client/DataHelper.h"
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
	AddProperty<int>(L"DestDepth", 1, L"Destination depth.");
	AddProperty<int>(L"Left", 0, L"X coordinate of top left corner of source data in destination data.");
	AddProperty<int>(L"Top", 0, L"Y coordinate of top left corner of source data in destination data.");
	AddProperty<int>(L"Front", 0, L"Z coordinate of front top left corner of source data in destination data.");
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

	DataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble && input_data.GetDataType() != DataTypeComplexFloat)
		return false;

	unsigned int dest_width(GetProperty<int>(L"DestWidth"));
	unsigned int dest_height(GetProperty<int>(L"DestHeight"));
	unsigned int dest_depth(GetProperty<int>(L"DestDepth"));
	unsigned int front = GetProperty<int>(L"Front");

	unsigned int input_width{ 1 }, input_height{ 1 }, input_depth{ 1 };
	if (input_data.GetDimensionCount() >= 1)
	{
		if (dest_width < input_data.GetWidth())
			return false;

		if (input_data.GetDimensionCount() >= 2)
		{
			if (dest_height < input_data.GetHeight())
				return false;

			if (input_data.GetDimensionCount() >= 3)
			{
				if (dest_depth < input_data.GetSliceCount() + front || front < 0)
					return false;
				input_depth = input_data.GetSliceCount();
			}
			input_height = input_data.GetHeight();
		}
		input_width = input_data.GetWidth();
	}
	else
		return false;

	Yap::Dimensions dims;
	dims(DimensionReadout, 0, dest_width)
		(DimensionPhaseEncoding, 0, dest_height)
		(DimensionSlice, 0, dest_depth);

	auto dest_size = dest_height * dest_width;
	auto source_size = input_width * input_height;
	if (data->GetDataType() == DataTypeComplexDouble)
	{
		auto output = CreateData<std::complex<double>>(data, &dims);
		memset(Yap::GetDataArray<complex<double>>(output.get()), 0, 
			dest_width * dest_height * dest_depth * sizeof(complex<double>));

		for (unsigned int slice = 0; slice < input_depth; ++slice)
		{
			zero_filling(Yap::GetDataArray<complex<double>>(output.get()) + dest_size * (slice + front), 
				dest_width, dest_height,
				Yap::GetDataArray<complex<double>>(data) + source_size * slice, 
				input_width, input_height);
		}

		return Feed(L"Output", output.get());
	}
	else
	{
		auto output = CreateData<std::complex<float>>(data, &dims);
		memset(Yap::GetDataArray<complex<float>>(output.get()), 0,
			dest_width * dest_height * dest_depth * sizeof(complex<float>));

		for (unsigned int slice = 0; slice < input_depth; ++slice)
		{
			zero_filling(Yap::GetDataArray<complex<float>>(output.get()) + dest_size * (slice + front),
				dest_width, dest_height,
				Yap::GetDataArray<complex<float>>(data) + source_size * slice,
				input_width, input_height);
		}
		return Feed(L"Output", output.get());
	}	
}