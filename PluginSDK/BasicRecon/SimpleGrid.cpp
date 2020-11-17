#include "stdafx.h"
#include "SimpleGrid.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <string>
#include <complex>

using namespace std;
using namespace Yap;
const double PI = 3.1415926535897932384626433832795028841971; // 圆周率

SimpleGrid::SimpleGrid() : ProcessorImpl(L"SimpleGrid")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddProperty<bool>(L"LinearRadial", true, L"Linear Radial method.");
	AddProperty<int>(L"DestWidth", 512, L"Destination width.");
	AddProperty<int>(L"DestHeight", 512, L"Destination height.");


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
	
	int radial_columns = helper.GetWidth();
	int radial_lines = helper.GetHeight();
	int src_depth = helper.GetSliceCount();

	bool LinearRadial( GetProperty<bool>(L"LinearRadial") );
	//Grid parameters:
	//1. delta_k: 缺省值2，表示重采样的放大倍数；
	//2. width, height, 重采样构造的k空间数据宽度和高度，跟radial_length, delta
	//必须满足一定的约束关系；
	float delta_k = 2;
	float delta_angle = LinearRadial ? PI / radial_lines : (111.25 / 180 * PI);
	int center_column_index = radial_columns / 2;// 430;//start from 0,
	int center_line_index = radial_lines / 2; // 320;//start from 0,
	int center_part_index = src_depth / 2;    // 21; //start from 0, slice direction, used in 3D FFT(?)
		
	int dest_width(GetProperty<int>(L"DestWidth")); 
	int dest_height(GetProperty<int>(L"DestHeight")); 
	int dest_depth = src_depth;
	{//validation check:
		assert(dest_width >= static_cast<int>(radial_columns * delta_k) &&
			L"dest_width should be greater than columns * delta_k!");
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
	}
	else
	{
		auto output = CreateData<std::complex<float>>(data, &output_dimensions);
	
		for (int slice_index = 0; slice_index < dest_depth; ++slice_index)
		{
			std::complex<float>* slice =
				Yap::GetDataArray<complex<float>>(output.get())
				+ dest_width*dest_height * slice_index;
			{//test.
				//for (int i = 0; i < dest_width; i++)
				//{
				//	*(slice + 100 * dest_width + i) = complex<float>(3, 4);
				//	*(slice + 450 * dest_width + i) = complex<float>(3, 4);
				//}
			}


			for (int line_index = 0; line_index < radial_lines; ++line_index)
			{
				std::complex<float>* radial =
					Yap::GetDataArray<complex<float>>(data)
					+ radial_columns * radial_lines * slice_index
					+ radial_columns * line_index;
				//猜测1：假定步进角度从 - pi / 2到pi / 2；
				float angle = (line_index - center_line_index) * delta_angle;
		
				
				FillKSpace(slice, dest_width, dest_height,
					       radial, radial_columns, angle, center_column_index, delta_k);
			}
			
		}

		{//log

			wstring info = wstring(L"<SimpleGrid>") + L"::Feed "
				+ L"----radial columns = " + to_wstring(radial_columns)
				+ L"----radial lines = " + to_wstring(radial_lines)
				+ L"----slice count = " + to_wstring(src_depth);
			LOG_TRACE(info.c_str(), L"BasicRecon");
		}
		return Feed(L"Output", output.get());
	}
}

//径向参数：center:回波中心点索引--该点跟k空间原点重合；
//假定k空间原点对应数据索引width/2和height/2
void SimpleGrid::FillKSpace(complex<float> * slice, int width, int height,
	complex<float> * radial, int columns, float angle, float center, float delta_k)
{
	for (int i = 0; i < columns; i++)
	{
		//猜测2：假定径向线上的步进点从-R到+R;
		float r = (i - center)*delta_k;
		float x, y;
		if (r > 0)
		{
			x = r * cos(angle+PI);
			y = r * sin(angle+PI);
		}
		else
		{
			x = -r * cos(angle);
			y = -r * sin(angle);
		}

		int x1 = floor(x + 0.001);
		int y1 = floor(y + 0.001);

		int x_index =  x1 + width / 2;
		int y_index = -y1 + height / 2;
		x_index = x_index >= 0 ? x_index : 0;
		y_index = y_index >= 0 ? y_index : 0;

		x_index = x_index >= width ? (width - 1) : x_index;
		y_index = y_index >= height ? (height - 1) : y_index;

		*(slice + y_index * width + x_index) = *(radial + i);//note: complex data.
	}
	int x = 3;
	return;
}
