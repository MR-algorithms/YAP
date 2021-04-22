#include "stdafx.h"
#include "SimpleGrid.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <string>
#include <complex>
#include <cassert>

using namespace std;
using namespace Yap;
const double PI = 3.1415926535897932384626433832795028841971; 

SimpleGrid::SimpleGrid() : ProcessorImpl(L"SimpleGrid")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddProperty<bool>(L"LinearRadial", true, L"Linear Radial method.");
	AddProperty<bool>(L"OnlyKPosition", true, L"Only k position.");
	AddProperty<bool>(L"RadialData", true, L"Radial Data.");
	AddProperty<int>(L"DestWidth", 0, L"Destination width.");
	AddProperty<int>(L"DestHeight", 0, L"Destination height.");


}

SimpleGrid::SimpleGrid(const SimpleGrid& rhs)
	:ProcessorImpl(rhs)
{
}

SimpleGrid::~SimpleGrid()
{
}
/**
	\remark backup function.
*/

/**
	\note 
	
*/
//测试：把样本标准k空间数据看成径向采集数据，径向采集线信息（采集开关和采集角度）记录在末点上。
void SimpleGrid::TestSetup(IData * data)
{
	assert((data != nullptr) && 
		(Yap::GetDataArray<complex<float>>(data) != nullptr) );
	
	DataHelper helper(data);

	int width = helper.GetWidth();
	int height = helper.GetHeight();
	unsigned int slice_block_size = helper.GetBlockSize(DimensionSlice);

	Dimension slice_dimension = helper.GetDimension(DimensionSlice);
	//----------
	//
	bool LinearRadial(GetProperty<bool>(L"LinearRadial"));
	float delta_angle = LinearRadial ? PI / height : (111.25 / 180 * PI);
	int center_line_index = height / 2; 

	//
	for (unsigned int i = slice_dimension.start_index; i < slice_dimension.start_index + slice_dimension.length; ++i)
	{
		float angles[256];
		if (helper.GetDataType() == DataTypeComplexFloat)
		{
			complex<float> *slice = Yap::GetDataArray<complex<float>>(data) +
				(i - slice_dimension.start_index) * slice_block_size;
			//
			int interleaves = 1;
			int total_turns = height / interleaves;
			//测试，仅重现部分采集线。
			for (int j = 0; j < height; j++)
			{
				*(slice + j * width + width - 1) = complex<float>(0,0);
			}
		
			for (int j = 0; j < height/3; j++)
			{
				//
				int turn_index = j % total_turns;
				//假定totoal_turns = 10, interleaves =4; 那么：
				//j = 2: turns_index = 2; n0 = 0;
				//j = 12: turn_index = 2; n0 = 1;
				int n0 = j / total_turns;
				//猜测步骤2：
				float angle1 = (turn_index * interleaves + n0) * delta_angle;
				//猜测步骤3：假定步进角度从 pi / 2到-pi / 2；
				float angle = (PI / 2 - angle1);
			
				*(slice + j * width + width - 1) = complex<float>(1.0, angle);


			}
			int x = 3;
			
		}
		else
		{
			assert(0 && L"Not Implemented.");
		}

	}

}
//本处理器为专用处理器：重现径向采集数据（即简单网格化处理）；
//1，dim1: columns,表示每条线上的数据，最后一个数据：实部表示是否采集，虚部表示采集的k空间角度；
//2, 参数空间中数据：
//--radial_scan: 表示是否为径向采集数据。
//--center_line_index: 中间线索引；表示对应ky=0的径向线索引
//--cneter_column_index：中间点索引：表示对应k空间原点的径向线上的索引点。
//Note:
//1, 对每一个角度：是用终点表征角度的；如：Gx,Gy为正值，角度为第一象限角，k空间位置从
//第三象限步进到第一象限
//2, 因为每条线上的末点数据有是否采集和角度信息，真实数据有这个信息，Yap加载数据测试没有这个信息，
//--所以LinearRadial: 表示线性角或黄金角径向--用于仿真设置角度信息。

bool SimpleGrid::Input(const wchar_t * port, IData * data)
{
	
	if (std::wstring(port) != L"Input")
	{
		LOG_ERROR(L"<SimpleGrid> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper helper(data);
	if (helper.GetDataType() != DataTypeComplexDouble && helper.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<SimpleGrid> Error input data type!(DataTypeComplexDouble and DataTypeComplexFloat are available)", L"BasicRecon");
		return false;
	}

	Dimensions data_dimensions(data->GetDimensions());
	//仿真设置径向采集信息，用于Yap内直接加载数据测试。
	if (!GetProperty<bool>(L"RadialData"))
	{
		TestSetup(data);
		assert(256 == helper.GetWidth());//Yap2内测试样本数据；
	}
	
	int src_width = helper.GetWidth();
	int line_count = helper.GetHeight();
	int src_depth = helper.GetSliceCount();

	bool OnlyKPosition(GetProperty<bool>(L"OnlyKPosition"));
	
	//Grid parameters:
	//0. 这里已知径向线重构k空间。
	//1. delta_k: 缺省值2，表示径向线在k空间中采样的间距；
	//2. width, height--k空间数据的宽度和高度，由已经的径向线决定：径向线点数，中心位置，径向线间距
	//必须满足一定的约束关系；
	
	int center_column_index = src_width / 2;
	int center_part_index = src_depth / 2;    // 21; //start from 0, slice direction, used in 3D FFT(?)
		
	int dest_width(GetProperty<int>(L"DestWidth")); 
	int dest_height(GetProperty<int>(L"DestHeight")); 
	int dest_depth = src_depth;
	assert(dest_width == dest_height);
	assert(dest_width > 0);
	float delta_k = static_cast<float>(dest_width) / static_cast<float>(src_width);
	
	
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
	}
	else
	{
		auto output = CreateData<std::complex<float>>(data, &output_dimensions);
	
		for (int slice_index = 0; slice_index < dest_depth; ++slice_index)
		{
			std::complex<float>* slice =
				Yap::GetDataArray<complex<float>>(output.get())
				+ dest_width*dest_height * slice_index;
			if (1)
			{

				for (int line_index = 0; line_index < line_count; ++line_index)
				{
					std::complex<float>* radial =
						Yap::GetDataArray<complex<float>>(data)
						+ src_width * line_count * slice_index
						+ src_width * line_index;

					complex<float> temp = *(radial + src_width - 1);
					bool gate = fabs(temp.real()) > 0.5;

					if (gate) {
						float angle = temp.imag();
						if (slice_index == 0)
						{
							//wstring info = wstring(L"<SimpleGrid>") + L"::Input "
							//	+ L"----slice_index = " + to_wstring(slice_index)
							//	+ L"----line_index = " + to_wstring(line_index)
							//	+ L"----line_angle = " + to_wstring(angle);
							//LOG_TRACE(info.c_str(), L"BasicRecon");
						}

						FillKSpace(slice, dest_width, dest_height,
							radial, src_width - 1, angle, center_column_index, delta_k, OnlyKPosition);
					}
				}

			}
			else {
				//test.确认k空间索引和图像的关系。
				//图像左上角为图像索引的原点。
				//通常：k空间数据存放方式（跟采样方式相关）是：随着索引增加
				for (int i = 0; i < dest_width*0.75; i++)
				{
					*(slice + 10 * dest_width + i) = complex<float>(3, 4);
					*(slice + 100 * dest_width + i) = complex<float>(3, 4);
				}
				
			}
		}

		{//log

			wstring info = wstring(L"<SimpleGrid>") + L"::Feed "
				+ L"----columns = " + to_wstring(src_width - 1)
				+ L"----line count = " + to_wstring(line_count)
				+ L"----slice count = " + to_wstring(src_depth);
			LOG_TRACE(info.c_str(), L"BasicRecon");
		}
		return Feed(L"Output", output.get());
	}
}

//径向参数：center:回波中心点索引--该点跟k空间原点重合；
//假定k空间原点对应数据索引width/2和height/2
void SimpleGrid::FillKSpace(complex<float> * slice, int width, int height,
	complex<float> * radial, int columns, float angle, float center, float delta_k, bool OnlyKPosition)
{
	float tempx = floor(-5.5);
	for (int i = 0; i < columns; i++)
	{
		//径向采集线的角度是由k空间径向线上的末点决定的；所以，
		//--径向角angle为锐角时，从第三象限往第一象限采样；
		//--径向角angle为-40度时，从第二象限往第四象限采样；
		float r = (i - center)*delta_k;
		float x, y;//x,y表示k空间坐标
		//r值从负值开始步进，但实际上采用极坐标算法，r应为非负值，负值实际表示第二三象限，应该角度做适当调整。
		if (r > 0)
		{
			x = r * cos(angle);
			y = r * sin(angle);
		}
		else
		{
			x = -r * cos(angle+PI);
			y = -r * sin(angle+PI);
		}
		//转换成数据索引坐标。
		int x1 = floor(x + 0.001);
		int y1 = floor(y + 0.001);

		int x_index =  x1 + width / 2;
		int y_index =  y1 + height / 2;
		x_index = x_index >= 0 ? x_index : 0;
		y_index = y_index >= 0 ? y_index : 0;

		x_index = x_index >= width-1 ? (width - 1) : x_index;
		y_index = y_index >= height-1 ? (height - 1) : y_index;

		*(slice + y_index * width + x_index) = OnlyKPosition? complex<float>(100,100): *(radial + i);//note: complex data.
		//数据驱动：每个数据多填充周围几个点。
		/*
		{//2
			int x1a = x1 + 1;
			int y1a = y1;
			int x_index = x1a + width / 2;
			int y_index = y1a + height / 2;
			x_index = x_index >= 0 ? x_index : 0;
			y_index = y_index >= 0 ? y_index : 0;

			x_index = x_index >= width - 1 ? (width - 1) : x_index;
			y_index = y_index >= height - 1 ? (height - 1) : y_index;

			*(slice + y_index * width + x_index) = OnlyKPosition ? complex<float>(100, 100) : *(radial + i);//note: complex data.
		}
		{//3
			int x1a = x1;
			int y1a = y1+1;
			int x_index = x1a + width / 2;
			int y_index = y1a + height / 2;
			x_index = x_index >= 0 ? x_index : 0;
			y_index = y_index >= 0 ? y_index : 0;

			x_index = x_index >= width - 1 ? (width - 1) : x_index;
			y_index = y_index >= height - 1 ? (height - 1) : y_index;

			*(slice + y_index * width + x_index) = OnlyKPosition ? complex<float>(100, 100) : *(radial + i);//note: complex data.
		}
		{//4
			int x1a = x1 + 1;
			int y1a = y1 + 1;
			int x_index = x1a + width / 2;
			int y_index = y1a + height / 2;
			x_index = x_index >= 0 ? x_index : 0;
			y_index = y_index >= 0 ? y_index : 0;

			x_index = x_index >= width - 1 ? (width - 1) : x_index;
			y_index = y_index >= height - 1 ? (height - 1) : y_index;

			*(slice + y_index * width + x_index) = OnlyKPosition ? complex<float>(100, 100) : *(radial + i);//note: complex data.
		}
		*/
	}
	int x = 3;
	return;
}
