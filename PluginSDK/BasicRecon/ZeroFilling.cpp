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
				 int source_height,
				 int left,
				 int top)
{
	assert(dest != nullptr && source != nullptr && left >= 0 && top >= 0);
	assert(dest_width >= source_width + left && dest_height >= source_height + top);

	for (int row = 0; row < source_height; ++row)
	{
		memcpy(dest + (top + row) * dest_width + left, source + row * source_width, source_width * sizeof(T));
	}

	return true;
}

ZeroFilling::ZeroFilling() : ProcessorImpl(L"ZeroFilling")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);

	AddProperty<int>(L"DestWidth", 0, L"Destination width.");
	AddProperty<int>(L"DestHeight", 0, L"Destination height.");
	AddProperty<int>(L"DestDepth", 0, L"Destination depth.");
	AddProperty<int>(L"Left", -1, L"X coordinate of top left corner of source data in destination data.(-1 means source data locate at center in destination)");
	AddProperty<int>(L"Top", -1, L"Y coordinate of top left corner of source data in destination data.(-1 means source data locate at center in destination)");
	AddProperty<int>(L"Front", -1, L"Z coordinate of front top left corner of source data in destination data.(-1 means source data locate at center in destination)");
}

ZeroFilling::ZeroFilling(const ZeroFilling& rhs)
	:ProcessorImpl(rhs)
{
}

ZeroFilling::~ZeroFilling()
{
}
/**
	\remark backup function.
*/
/*
bool ZeroFilling::Input_backup(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
	{
		LOG_ERROR(L"<ZeroFilling> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexDouble && input_data.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<ZeroFilling> Error input data type!(DataTypeComplexDouble and DataTypeComplexFloat are available)", L"BasicRecon");
		return false;
	}

	int dest_width(GetProperty<int>(L"DestWidth"));
	if (dest_width < 0)
	{
		LOG_ERROR(L"<ZeroFilling> DestWidth cannot less than 0!", L"BasicRecon");
		return false;
	}

	unsigned int dest_height(GetProperty<int>(L"DestHeight"));
	if (dest_height < 0)
	{
		LOG_ERROR(L"<ZeroFilling> DestHeight cannot less than 0!", L"BasicRecon");
		return false;
	}

	unsigned int dest_depth(GetProperty<int>(L"DestDepth"));
	if (dest_depth < 0)
	{
		LOG_ERROR(L"<ZeroFilling> DestDepth cannot less than 0!", L"BasicRecon");
		return false;
	}

	int left = GetProperty<int>(L"Left");
	int top = GetProperty<int>(L"Top");
	int front = GetProperty<int>(L"Front");
	Yap::Dimensions dims;
	int input_width{ 1 }, input_height{ 1 }, input_depth{ 1 };
	if (input_data.GetDimensionCount() >= 1)
	{
		input_width = input_data.GetWidth();

		if (dest_width == 0)
		{
			dest_width = input_width;
		}
		if (left < 0)
		{
			left = (dest_width - input_data.GetWidth()) / 2;
		}

		if (dest_width < (int)input_data.GetWidth() + left || left < 0)
		{
			LOG_ERROR(L"<ZeroFilling> Improper property DestWidth!(DestWidth should larger than sum of source width and property Left)", L"BasicRecon");
			return false;
		}

		// Dim >= 2
		if (input_data.GetDimensionCount() >= 2)
		{
			input_height = input_data.GetHeight();

			if (dest_height == 0)
			{
				dest_height = input_height;
			}
			if (top < 0)
			{
				top = (dest_height - input_data.GetHeight()) / 2;
			}

			if (dest_height < input_data.GetHeight() + top || top < 0)
			{
				LOG_ERROR(L"<ZeroFilling> Improper property DestHeight!(DestHeight should larger than sum of source height and property Top)", L"BasicRecon");
				return false;
			}

			// Dim = 3
			if (input_data.GetDimensionCount() >= 3)
			{
				input_depth = input_data.GetSliceCount();

				if (dest_depth == 0)
				{
					dest_depth = input_depth;
				}
				if (front < 0)
				{
					front = (dest_depth - input_data.GetSliceCount()) / 2;
				}

				if (dest_depth < input_data.GetSliceCount() + front || front < 0)
				{
					LOG_ERROR(L"<ZeroFilling> Improper property DestDepth!(DestDepth should larger than sum of source slice count and property Front)", L"BasicRecon");
					return false;
				}
				dims(DimensionReadout, 0, dest_width)
					(DimensionPhaseEncoding, 0, dest_height)
					(DimensionSlice, 0, dest_depth);
			}
			else
			{
				front = 0;
				dims(DimensionReadout, 0, dest_width)
					(DimensionPhaseEncoding, 0, dest_height);
			}
		}
		else
		{
			top = 0;
			front = 0;
			dims(DimensionReadout, 0, dest_width);
		}
	}
	else
	{
		LOG_ERROR(L"<ZeroFilling> Error input data dimension!(1D, 2D, 3D are available)", L"BasicRecon");
		return false;
	}

	auto dest_size = dest_height * dest_width;
	auto source_size = input_width * input_height;
	
	Dimensions output_dimensions(data->GetDimensions());
	output_dimensions.Combine(&dims);

	if (data->GetDataType() == DataTypeComplexDouble)
	{
		auto output = CreateData<std::complex<double>>(data, &output_dimensions);
		memset(Yap::GetDataArray<complex<double>>(output.get()), 0, 
			dest_width * dest_height * dest_depth * sizeof(complex<double>));

		for (int slice = 0; slice < input_depth; ++slice)
		{
			zero_filling(Yap::GetDataArray<complex<double>>(output.get()) + dest_size * (slice + front), 
				dest_width, dest_height,
				Yap::GetDataArray<complex<double>>(data) + source_size * slice, 
				input_width, input_height, left, top);
		}

		return Feed(L"Output", output.get());
	}
	else
	{
		auto output = CreateData<std::complex<float>>(data, &output_dimensions);
		memset(Yap::GetDataArray<complex<float>>(output.get()), 0,
			dest_width * dest_height * dest_depth * sizeof(complex<float>));

		for (int slice = 0; slice < input_depth; ++slice)
		{
			zero_filling(Yap::GetDataArray<complex<float>>(output.get()) + dest_size * (slice + front),
				dest_width, dest_height,
				Yap::GetDataArray<complex<float>>(data) + source_size * slice,
				input_width, input_height, left, top);
		}
		return Feed(L"Output", output.get());
	}	
}
*/
unsigned int ZeroFilling::GetFillingCount(IData* data, int dest_width, int dest_height, int dest_depth)
{
	
	//Calculte dimension count.

	DataHelper helper(data);
	
	unsigned int dimension_count = -1;
	
	if (dest_width > 0 && dest_height <= 0 && dest_depth <= 0
		&& helper.GetDimension(DimensionReadout).type == DimensionReadout)
	{
		dimension_count = 1;

	}
	else if (dest_width > 0 && dest_height > 0 && dest_depth <= 0
		&&helper.GetDimension(DimensionReadout).type == DimensionReadout
		&&helper.GetDimension(DimensionPhaseEncoding).type == DimensionPhaseEncoding)//
	{
		dimension_count = 2;

	}
	else if (dest_width > 0 && dest_height > 0 && dest_depth > 0
		&& helper.GetDimension(DimensionReadout).type == DimensionReadout
		&& helper.GetDimension(DimensionPhaseEncoding).type == DimensionPhaseEncoding
		&& helper.GetDimension(DimensionSlice).type == DimensionSlice)
	{
		dimension_count = 3;
	}
	else
	{
		assert(0);
	}

	return dimension_count;
}


void ZeroFilling::UpdateStartLength(const int input_length, int &dest_start, int &dest_length, bool same)
{
	if (!same)
	{
		if (dest_length == 0)
		{
			dest_length = input_length;
		}
		if (dest_start < 0)
		{
			dest_start = (dest_length - input_length) / 2;
		}
	}
	else
	{
		dest_start = 0;
		dest_length = input_length;
	}
	
}
/**
	\remark rewrite according to Input_backup.
	1, 1D Do 1D when width >0 && height <= 0 && depth <= 0 and there is actual readout dimension. And it should not change other dimensions.
	2, 1D Do 2D when width >0 && height > 0  && depth <= 0 and there is actual phase encoding dimension. And it should not change other dimensions.
	3, 1D Do 3D when width >0 && height > 0  && depth > 0  and there is actual slice dimension. And it should not change other dimensions.
	3D Maybe be used in case of multislab in 3D imaging, the third dimension (slice dimension) denote the slice count of one slab.
	\note Only zero filling in 2D is supported and tested.
*/
bool ZeroFilling::Input(const wchar_t * port, IData * data)
{
	
	if (std::wstring(port) != L"Input")
	{
		LOG_ERROR(L"<ZeroFilling> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper helper(data);
	if (helper.GetDataType() != DataTypeComplexDouble && helper.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<ZeroFilling> Error input data type!(DataTypeComplexDouble and DataTypeComplexFloat are available)", L"BasicRecon");
		return false;
	}

	int dest_width(GetProperty<int>(L"DestWidth"));
	if (dest_width < 0)
	{
		LOG_ERROR(L"<ZeroFilling> DestWidth cannot less than 0!", L"BasicRecon");
		return false;
	}

	int dest_height(GetProperty<int>(L"DestHeight"));
	if (dest_height < 0)
	{
		LOG_ERROR(L"<ZeroFilling> DestHeight cannot less than 0!", L"BasicRecon");
		return false;
	}

	int dest_depth(GetProperty<int>(L"DestDepth"));
	if (dest_depth < 0)
	{
		LOG_ERROR(L"<ZeroFilling> DestDepth cannot less than 0!", L"BasicRecon");
		return false;
	}
	
	int left = GetProperty<int>(L"Left");
	int top = GetProperty<int>(L"Top");
	int front = GetProperty<int>(L"Front");
	Yap::Dimensions dims;
	int input_width{ 1 }, input_height{ 1 }, input_depth{ 1 };

	switch (GetFillingCount(data, dest_width, dest_height, dest_depth))
	{
	case 1:
		input_width = helper.GetWidth();
		input_height = helper.GetHeight();
		input_depth = helper.GetSliceCount();

		UpdateStartLength(input_width, left, dest_width);
		UpdateStartLength(input_height, top, dest_height, true);
		UpdateStartLength(input_depth, front, dest_depth, true);

		if (dest_width < (int)helper.GetWidth() + left || left < 0)
		{
			LOG_ERROR(L"<ZeroFilling> Improper property DestWidth!(DestWidth should larger than sum of source width and property Left)", L"BasicRecon");
			return false;
		}
		
		dims(DimensionReadout, 0, dest_width);

		break;
	case 2:
		input_width = helper.GetWidth();
		input_height = helper.GetHeight();
		input_depth = helper.GetSliceCount();
	
		UpdateStartLength(input_width, left, dest_width);
		UpdateStartLength(input_height, top, dest_height);
		UpdateStartLength(input_depth, front, dest_depth, true);
		
		if (dest_width < (int)helper.GetWidth() + left || left < 0)
		{
			LOG_ERROR(L"<ZeroFilling> Improper property DestWidth!(DestWidth should larger than sum of source width and property Left)", L"BasicRecon");
			return false;
		}

		if (dest_height < helper.GetHeight() + top || top < 0)
		{
			LOG_ERROR(L"<ZeroFilling> Improper property DestHeight!(DestHeight should larger than sum of source height and property Top)", L"BasicRecon");
			return false;
		}

		
		dims(DimensionReadout, 0, dest_width)
			(DimensionPhaseEncoding, 0, dest_height);

		break;
	case 3:
		
		input_width = helper.GetWidth();
		input_height = helper.GetHeight();
		input_depth = helper.GetSliceCount();

		UpdateStartLength(input_width, left, dest_width);
		UpdateStartLength(input_height, top, dest_height);
		UpdateStartLength(input_depth, front, dest_depth);
	
		//
		if (dest_width < (int)helper.GetWidth() + left || left < 0)
		{
			LOG_ERROR(L"<ZeroFilling> Improper property DestWidth!(DestWidth should larger than sum of source width and property Left)", L"BasicRecon");
			return false;
		}

		if (dest_height < helper.GetHeight() + top || top < 0)
		{
			LOG_ERROR(L"<ZeroFilling> Improper property DestHeight!(DestHeight should larger than sum of source height and property Top)", L"BasicRecon");
			return false;
		}


		if (dest_depth < helper.GetSliceCount() + front || front < 0)
		{
			LOG_ERROR(L"<ZeroFilling> Improper property DestDepth!(DestDepth should larger than sum of source slice count and property Front)", L"BasicRecon");
			return false;
		}

		dims(DimensionReadout, 0, dest_width)
			(DimensionPhaseEncoding, 0, dest_height)
			(DimensionSlice, 0, dest_depth);
		break;
	
	default:
		LOG_ERROR(L"<ZeroFilling> Error input data dimension!(1D, 2D, 3D are available)", L"BasicRecon");
		break;
	}

	

	auto dest_size = dest_height * dest_width;
	auto source_size = input_width * input_height;

	Dimensions output_dimensions(data->GetDimensions());
	output_dimensions.Combine(&dims);

	if (data->GetDataType() == DataTypeComplexDouble)
	{
		auto output = CreateData<std::complex<double>>(data, &output_dimensions);
		memset(Yap::GetDataArray<complex<double>>(output.get()), 0,
			dest_width * dest_height * dest_depth * sizeof(complex<double>));

		for (int slice = 0; slice < input_depth; ++slice)
		{
			zero_filling(Yap::GetDataArray<complex<double>>(output.get()) + dest_size * (slice + front),
				dest_width, dest_height,
				Yap::GetDataArray<complex<double>>(data) + source_size * slice,
				input_width, input_height, left, top);
		}

		return Feed(L"Output", output.get());
	}
	else
	{
		auto output = CreateData<std::complex<float>>(data, &output_dimensions);
		memset(Yap::GetDataArray<complex<float>>(output.get()), 0,
			dest_width * dest_height * dest_depth * sizeof(complex<float>));

		for (int slice = 0; slice < input_depth; ++slice)
		{
			zero_filling(Yap::GetDataArray<complex<float>>(output.get()) + dest_size * (slice + front),
				dest_width, dest_height,
				Yap::GetDataArray<complex<float>>(data) + source_size * slice,
				input_width, input_height, left, top);
		}
		return Feed(L"Output", output.get());
	}
}