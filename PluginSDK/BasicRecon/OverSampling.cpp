#include "stdafx.h"
#include "OverSampling.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <string>
#include <complex>

using namespace std;
using namespace Yap;
//left and top denotes the start point of the source data, which to be copied.
template <typename T>
bool clip(T* dest,
	int dest_width,
	int dest_height,
	T* source,
	int source_width,
	int source_height,
	int left,
	int top)
{
	assert(dest != nullptr && source != nullptr && left >= 0 && top >= 0);
	assert(source_width >= dest_width + left && source_height >= dest_height + top);

	for (int row = 0; row < dest_height; ++row)
	{
		memcpy(dest + row * dest_width, 
			source + (top + row) * source_width + left, dest_width * sizeof(T));
	}

	return true;
}

OverSampling::OverSampling() : ProcessorImpl(L"OverSampling")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddProperty<bool>(L"FreqOverSampling", false, L"Process Frequency OverSampling.");
	AddProperty<bool>(L"PhaseOverSampling", false, L"Process Phase OverSampling.");

}

OverSampling::OverSampling(const OverSampling& rhs)
	:ProcessorImpl(rhs)
{
}

OverSampling::~OverSampling()
{
}
/**
	\remark backup function.
*/


bool OverSampling::Input(const wchar_t * port, IData * data)
{

	if (std::wstring(port) != L"Input")
	{
		LOG_ERROR(L"<OverSampling> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper helper(data);
	if (helper.GetDataType() != DataTypeComplexDouble && helper.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<OverSampling> Error input data type!(DataTypeComplexDouble and DataTypeComplexFloat are available)", L"BasicRecon");
		return false;
	}

	

	bool FreqOverSampling(GetProperty<bool>(L"FreqOverSampling"));
	bool PhaseOverSampling(GetProperty<bool>(L"PhaseOverSampling"));
	if (!FreqOverSampling && !PhaseOverSampling)
	{
		return Feed(L"Output", data);
	}
	else 
	{
		
		//FreqInput(port, data);
		
		return ProcessOverSampling(port, data);
	}

}


/**
\note
//仅支持频率编码或相位编码方向2倍过采：FT完成后，把中间一半图像作为结果。
*/
bool OverSampling::ProcessOverSampling(const wchar_t * port, IData * data)
{
	DataHelper helper(data);
	
	int src_width{ 1 }, src_height{ 1 }, src_depth{ 1 };
	src_width = helper.GetWidth();
	src_height = helper.GetHeight();
	src_depth = helper.GetSliceCount();

	bool FreqOverSampling(GetProperty<bool>(L"FreqOverSampling"));
	bool PhaseOverSampling(GetProperty<bool>(L"PhaseOverSampling"));

	//------------
	
	int dest_width = src_width;
	int dest_height = src_height;
	int dest_depth = src_depth;
	if (FreqOverSampling)
	{
		dest_width /= 2;
	}
	if (PhaseOverSampling)
	{
		dest_height /= 2;
	}
	//
	Yap::Dimensions dims;

	dims(DimensionReadout, 0, dest_width)
		(DimensionPhaseEncoding, 0, dest_height);

	Dimensions output_dimensions(data->GetDimensions());
	output_dimensions.Combine(&dims);
	//----

	if (data->GetDataType() == DataTypeComplexDouble)
	{
		assert(0 && L"Not Implemented!");
		return false;
	}
	else
	{
		//-------
		auto output = CreateData<std::complex<float>>(data, &output_dimensions);
		memset(Yap::GetDataArray<complex<float>>(output.get()), 0,
			dest_width * dest_height * dest_depth * sizeof(complex<float>));

		for (int slice = 0; slice < src_depth; ++slice)
		{
			int left = (src_width-dest_width) / 2;
			int top = (src_height - dest_height) / 2;
			clip(Yap::GetDataArray<complex<float>>(output.get()),
				dest_width, dest_height,
				Yap::GetDataArray<complex<float>>(data), src_width, src_height, 
				left, top);
		}
		
		{//log

			wstring info = wstring(L"<OverSampling>") + L"::Feed -----source_width = " + to_wstring(src_width)
				+ L"----dest_width = " + to_wstring(dest_width);
			LOG_TRACE(info.c_str(), L"BasicRecon");
		}
		return Feed(L"Output", output.get());
	}
}

/**
\note
//仅支持读方向2倍过采：FT完成后，把中间一半图像作为结果。
*/
bool OverSampling::FreqInput(const wchar_t * port, IData * data)
{
	bool FreqOverSampling(GetProperty<bool>(L"FreqOverSampling"));
	bool PhaseOverSampling(GetProperty<bool>(L"PhaseOverSampling"));
	assert(FreqOverSampling && !PhaseOverSampling);

	DataHelper helper(data);
	Dimensions data_dimensions(data->GetDimensions());
	int src_width{ 1 }, src_height{ 1 }, src_depth{ 1 };
	src_width = helper.GetWidth();
	src_height = helper.GetHeight();
	src_depth = helper.GetSliceCount();

	//------------
	int dest_width = src_width / 2;//special frequency oversampling is supported.
	int dest_height = src_height;
	int dest_depth = src_depth;

	//
	Yap::Dimensions dims;
	dims(DimensionReadout, 0, dest_width);
	Dimensions output_dimensions(data->GetDimensions());
	output_dimensions.Combine(&dims);
	//----

	if (data->GetDataType() == DataTypeComplexDouble)
	{
		assert(0 && L"Not Implemented!");
		return false;
	}
	else
	{
		auto output = CreateData<std::complex<float>>(data, &output_dimensions);
		int dest_size = dest_width * dest_height;
		int src_size = src_width * src_height;
		memset(Yap::GetDataArray<complex<float>>(output.get()), 0,
			dest_width * dest_height * dest_depth * sizeof(complex<float>));


		for (int slice_index = 0; slice_index < dest_depth; ++slice_index)
		{
			for (int row_index = 0; row_index < dest_height; ++row_index)
			{
				memcpy(Yap::GetDataArray<complex<float>>(output.get()) + dest_size * slice_index
					+ row_index * dest_width,
					Yap::GetDataArray<complex<float>>(data) + src_size * slice_index
					+ row_index * src_width + (src_width - dest_width) / 2,
					dest_width * sizeof(complex<float>));
			}

		}

		{//log

			wstring info = wstring(L"<OverSampling>") + L"::Feed -----source_width = " + to_wstring(src_width)
				+ L"----dest_width = " + to_wstring(dest_width);
			LOG_TRACE(info.c_str(), L"BasicRecon");
		}
		return Feed(L"Output", output.get());
	}
}
