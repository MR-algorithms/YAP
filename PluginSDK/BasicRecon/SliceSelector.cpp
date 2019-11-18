#include "SliceSelector.h"
#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <complex>

using namespace std;
using namespace Yap;

SliceSelector::SliceSelector(void):
	ProcessorImpl(L"SliceSelector")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexFloat);

	AddProperty<int>(L"SliceIndex", 3, L"The index of the slice you want to get.");
	
}

Yap::SliceSelector::SliceSelector(const SliceSelector & rhs)
	: ProcessorImpl(rhs)
{
}

SliceSelector::~SliceSelector()
{
}
	
/**
	\remark 各个处理器普遍存在三种问题：
	1，数据维度转换时，信息丢失;比如：通道，回波索引，
	2，数据访问只考虑了简单情况;
	3，入口数据没有合理的合法性检查。
	本处理器：这种访问数据的方式是有条件的：实际维度等于三维等于1（比如通道当前数据的通道维度大小[不是索引].）
	另外一些参数需要专门考虑，如：Slice_index处理器参数空间、数据维度里必须有这个参数，数据参数空间是可选的，

*/

bool Yap::SliceSelector::Input(const wchar_t * name, IData * data)
{
	//Test VariableSpace.
	VariableSpace variable(data->GetVariables());

	//end of TESt.
	
	bool is_type_complexf = (Yap::GetDataArray<complex<float>>(data) != nullptr);
	bool is_type_short =	(Yap::GetDataArray<short>(data) != nullptr);

	assert((data != nullptr) && (is_type_complexf || is_type_short));
	assert(Inputs()->Find(name) != nullptr);
		
	assert(VariablesValid(data));
	int slice_index = GetProperty<int>(L"SliceIndex");

	//AddSliceindexParam(data, slice_index, DataHelper(data).GetDataType());
	
	DataHelper input_data(data);
	Dimensions data_dimentions(data->GetDimensions());
	unsigned int slice_block_size = input_data.GetBlockSize(DimensionSlice);
	if (input_data.GetActualDimensionCount() <= 3)
	{		
		data_dimentions.SetDimension(DimensionSlice, 1, slice_index);
		if (is_type_complexf)
		{
			
			auto output = CreateData<complex<float>>(data,
				Yap::GetDataArray<complex<float>>(data) + slice_index * slice_block_size, 
				data_dimentions, data);

			Feed(L"Output", output.get());
		}
		if(is_type_short)
		{
			auto output = CreateData<short>(data,
				Yap::GetDataArray<short>(data)
				+ slice_index * slice_block_size, data_dimentions, data);
			Feed(L"Output", output.get());
		}
	}
	else
	{
		Dimension channel_dimention = input_data.GetDimension(DimensionChannel);
		unsigned int channel_block_size = input_data.GetBlockSize(DimensionChannel);
		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, input_data.GetWidth())
			(DimensionPhaseEncoding, 0U, input_data.GetHeight())
			(DimensionSlice, slice_index, 1)
			(Dimension4, 0U, input_data.GetDim4())
			(DimensionChannel, 0U, input_data.GetCoilCount());

		if (is_type_complexf)
		{
			complex<float>* slice_channel_data = nullptr;
			try
			{
				slice_channel_data = new complex<float>[input_data.GetWidth() * input_data.GetHeight() * input_data.GetCoilCount()];
			}
			catch(bad_alloc&)
			{
				return false;
			}	
			for (unsigned int i = channel_dimention.start_index; i < channel_dimention.start_index + channel_dimention.length; ++i)
			{
				memcpy(slice_channel_data + i * slice_block_size, Yap::GetDataArray<complex<float>>(data) + slice_index * slice_block_size + i * channel_block_size, 
					slice_block_size * sizeof(complex<float>));
			}
			auto outdata = CreateData<complex<float>>(data, slice_channel_data, dimensions, nullptr);
			Feed(L"Output", outdata.get());
		}
		if(is_type_short)
		{
			short* slice_channel_data = nullptr;
			try
			{
				slice_channel_data = new  short[input_data.GetWidth() * input_data.GetHeight() * input_data.GetCoilCount()];
			}
			catch (bad_alloc&)
			{
				return false;
			}
			for (unsigned int i = channel_dimention.start_index; i < channel_dimention.start_index + channel_dimention.length; ++i)
			{
				memcpy(slice_channel_data + i * slice_block_size, Yap::GetDataArray<short>(data) + slice_index * slice_block_size + i * channel_block_size,
					slice_block_size * sizeof(short));
			}
			auto outdata = CreateData<short>(data, slice_channel_data, dimensions, nullptr);
			Feed(L"Output", outdata.get());
		}
	}
	return true;
}


void SliceSelector::TestChannelOnSlice(IData *output)
{
	auto temp = output->GetDimensions();
	Dimensions data_dimentions(output->GetDimensions());
	DataHelper helper(output);
	Dimension channel_dimention = helper.GetDimension(DimensionChannel);
	Dimension read_dimmention = helper.GetDimension(DimensionReadout);
	Dimension phase_dimention = helper.GetDimension(DimensionPhaseEncoding);

	
	assert(channel_dimention.length == 1);

	return;

}