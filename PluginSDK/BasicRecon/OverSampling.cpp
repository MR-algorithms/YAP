#include "stdafx.h"
#include "OverSampling.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <string>
#include <complex>

using namespace std;
using namespace Yap;

OverSampling::OverSampling() : ProcessorImpl(L"OverSampling")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddProperty<bool>(L"FreqOverSampling", false, L"Frequency OverSampling.");

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

/**
	\note 
	//仅支持读方向2倍过采情况：FT完成后，把中间一半图像作为结果。
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
		LOG_ERROR(L"<ZeroFilling> Error input data type!(DataTypeComplexDouble and DataTypeComplexFloat are available)", L"BasicRecon");
		return false;
	}

	Dimensions data_dimensions(data->GetDimensions());
	int src_width{ 1 }, src_height{ 1 }, src_depth{ 1 };
	src_width = helper.GetWidth();
	src_height = helper.GetHeight();
	src_depth = helper.GetSliceCount();

	bool FreqOverSampling( GetProperty<bool>(L"FreqOverSampling") );
	if (!FreqOverSampling)
	{
		return Feed(L"Output", data);
	}

	//
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
					+ row_index * src_width + (src_width - dest_width)/2,
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