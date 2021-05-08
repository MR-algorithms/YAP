#include "stdafx.h"
#include "RadialSampling.h"
#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

using namespace std;
using namespace Yap;
const double PI = 3.1415926535897932384626433832795028841971;

RadialSampling::RadialSampling():
	ProcessorImpl(L"RadialSampling")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexFloat);
	AddInput(L"Mask", 2, DataTypeFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexFloat);
}

RadialSampling::RadialSampling(const RadialSampling& rhs)
	:ProcessorImpl(rhs)
{
}

RadialSampling::~RadialSampling()
{
}

bool Yap::RadialSampling::Input(const wchar_t * port, IData * data)
{
	//-
	DataHelper helper(data);

	int width  = helper.GetWidth();
	int height = helper.GetHeight();
	int depth  = helper.GetSliceCount();
	//径向采集示例参数：
	bool LinearRadial = true; //switch between linear and golden angle.
	int radial_columns = width;
	int line_count = 100;
	float delta_k = 1.0;
	int center_line_index = line_count / 2;
	int center_column_index = radial_columns / 2;//start from 0, 
	//end of 参数示例.
	
	Yap::Dimensions dims;
	dims(DimensionReadout, 0, radial_columns)
		(DimensionPhaseEncoding, 0, line_count);

	Dimensions output_dimensions(data->GetDimensions());
	output_dimensions.Combine(&dims);

	//----

	auto output = CreateData<complex<float>>(data, &output_dimensions);
	memset(Yap::GetDataArray<complex<float>>(output.get()), 0,
		radial_columns * line_count * depth * sizeof(complex<float>));

	//暂不考虑interleave.
	float delta_angle = LinearRadial ? PI / line_count : (111.25 / 180 * PI);
	
	for (int slice_index = 0; slice_index < depth; ++slice_index)
	{
		complex<float>* slice_in  = Yap::GetDataArray<complex<float>>(data) + slice_index * width * height;
		complex<float>* slice_out = Yap::GetDataArray<complex<float>>(output.get()) + slice_index * radial_columns * line_count;

		for (int line_index = 0; line_index < line_count; ++line_index)
		{
			//设置1：径向采集线 步进角度从 - pi / 2到pi / 2
			float angle = (line_index - center_line_index) * delta_angle;

			
			boost::shared_array<complex<float>> aline
				= GetRadialLine(slice_in, width, height, radial_columns, angle, center_column_index, delta_k);
			memcpy(slice_out + line_index * radial_columns, aline.get(), radial_columns*sizeof(complex<float>));
			

		}
		
			
	}
	


	return true;
}

boost::shared_array<complex<float>>
RadialSampling::GetRadialLine(std::complex<float> * input_data2D,
	unsigned int width, unsigned int height,
	int columns, float angle, int center, float delta_k)
{
	
	boost::shared_array<complex<float>> radial_line(new complex<float>[columns]);
	//Sample the radial data.
	//
	for (int i = 0; i < columns; i++)
	{
		//1, 计算采样点坐标（坐标系：k空间）
		float r = (i - center)*delta_k;
		float x, y;
		if (r > 0)
		{
			x = r * cos(angle);
			y = r * sin(angle);
		}
		else
		{
			x = -r * cos(angle + PI);
			y = -r * sin(angle + PI);
		}

		//--------
		//2, 转换成像素坐标：左上角为原点：
		float x_index = x + width / 2;
		float y_index = y + height / 2;
		x_index = x_index >= 0 ? x_index : 0;
		y_index = y_index >= 0 ? y_index : 0;

		x_index = x_index >= width - 1 ? (width - 1) : x_index;
		y_index = y_index >= height - 1 ? (height - 1) : y_index;
		//3, 双线性插值：
		//假定采样点的临近四个点分别为f00,f01, f10, f11;
		int x0 = floor(x_index);
		int y0 = floor(y_index);

		//百度百科
		complex<float> f00 = *(input_data2D + y0 * width + x0);
		complex<float> f10 = *(input_data2D + y0 * width + x0 + 1);
		complex<float> f01 = *(input_data2D + (y0 + 1) * width + x0);
		complex<float> f11 = *(input_data2D + (y0 + 1) * width + x0 + 1);

		{
			float x = x_index - x0;
			float y = y_index - y0;
			complex<float> fxy = f00*(1 - x)*(1 - y) + f10*x*(1 - y) + f01*(1 - x)*y + f11*x*y;
			*(radial_line.get() + i) = fxy;
		}


	}

	//end of sample.
	return radial_line;
}
