#include "stdafx.h"
#include "DrawlineOnImage.h"
#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

using namespace std;
using namespace Yap;
const double PI = 3.1415926535897932384626433832795028841971;


DrawlineOnImage::DrawlineOnImage():
	ProcessorImpl(L"DrawlineOnImage")
{
	AddInput(L"Input", 2, DataTypeFloat | DataTypeUnsignedShort | DataTypeShort);
	AddOutput(L"Output", 2, DataTypeFloat | DataTypeUnsignedShort | DataTypeShort);
	
	AddProperty<double>(L"ScaleY", 0, L"Scale of Y.");
}

DrawlineOnImage::DrawlineOnImage(const DrawlineOnImage& rhs)
	:ProcessorImpl(rhs)
{
}

DrawlineOnImage::~DrawlineOnImage()
{
}

bool DrawlineOnImage::Input(const wchar_t * port, IData * data)
{
	//-
	DataHelper helper(data);

	int width  = helper.GetWidth();
	int height = helper.GetHeight();
	int depth  = helper.GetSliceCount();
	assert(depth == 1 &&L"Only support 1 slice");

	//
	
	double ScaleY(GetProperty<double>(L"ScaleY"));
	assert(ScaleY > 0.5 && ScaleY <= 1.0);

	//
	boost::shared_array<double> line = GetTestline(width / 2);
	
	switch (data->GetDataType())
	{
	case DataTypeFloat:
	{
		float* buffer = Yap::GetDataArray<float>(data);
		Drawline(buffer, width, height, line.get(), width / 2, ScaleY);

	}
		break;
	case DataTypeUnsignedShort:
	{
		unsigned short* buffer = Yap::GetDataArray<unsigned short>(data);
		Drawline(buffer, width, height, line.get(), width / 2, ScaleY);

	}
		break;
	case DataTypeShort:
	{
		short* buffer = Yap::GetDataArray<short>(data);
		Drawline(buffer, width, height, line.get(), width / 2, ScaleY);

	}
		break;
	default:
		assert(0);

	}
		
	
	
	return Feed(L"Output", data);
	
}

//把一维数据画在图像上（图像像素坐标：第0行第0列为原点；数据索引为横坐标、数据值为纵坐标）；
//这里并不关心图像怎么显示，如左上角还是左下角为原点。
template <typename IN_TYPE>
void DrawlineOnImage::Drawline(IN_TYPE* input_data2D, unsigned int width, unsigned int height,
	double* line1D, unsigned int size, double scaley)
{
	
	//
	auto result = std::minmax_element(line1D, line1D + size);
	double min_data = *result.first;
	double max_data = *result.second;
	//
	double max_abs = fabs(max_data) > fabs(min_data) ? fabs(max_data) : fabs(min_data);
	boost::shared_array<unsigned int> line_temp(new unsigned int[size]);
	//以中间行作为纵坐标原点，把曲线放大到图像边缘（scale用于调节高点是否触及边缘）。
	for (unsigned int i = 0; i < size; i++)
	{
		unsigned int temp = static_cast<unsigned int>( *(line1D + i) * (height / 2) / max_abs * scaley );
		*(line_temp.get() + i) = temp;
		
	}
	//坐标变换：变换到图像像素坐标；
	assert(width >= size);
	unsigned int deltax_index = (width - size) / 2;
	unsigned int deltay_index = height / 2;

	auto result2 = std::minmax_element(input_data2D, input_data2D + width*height);

	auto min_of_input = *result2.first;
	auto max_of_input = *result2.second;
	vector<SimplePoint<unsigned int>> points;
	for (unsigned int i = 0; i < size; i++)
	{
		unsigned int x_index = deltax_index + i;
		unsigned int y_index = *(line_temp.get() + i) + deltay_index;
		points.push_back(SimplePoint<unsigned int>(x_index, y_index));
		//*(input_data2D + width * y_index + x_index) = max_of_input;
	}

	for (unsigned int i = 0; i < size - 1; i++)
	{
		LineTo(input_data2D, width, height,
			points[i].x, points[i].y,
			points[i + 1].x, points[i + 1].y);
	}
	int x = 4;
	
}

template <typename IN_TYPE>
void DrawlineOnImage::SetPixel(IN_TYPE* input_data2D, unsigned int width, unsigned int height, unsigned int x, unsigned int y, IN_TYPE value)
{
	*(input_data2D + width*y + x) = value;

}
template <typename IN_TYPE>
void DrawlineOnImage::LineTo(IN_TYPE* input_data2D, unsigned int width, unsigned int height, int x1, int y1, int x2, int y2)
{
	auto result2 = std::minmax_element(input_data2D, input_data2D + width*height);

	auto min_of_input = *result2.first;
	auto max_of_input = *result2.second;

	//
	unsigned int deltax = abs(x2 - x1);
	unsigned int deltay = abs(y2 - y1);
	unsigned int steps = deltax > deltay ? deltax : deltay;
	
	//for (unsigned int i = 0; i < steps; i++)
	//{
	//	unsigned int x = x1 + static_cast<double>(i) / steps* (x2-x1) + 0.5;
	//	unsigned int y = y1 + static_cast<double>(i) / steps* (y2-y1) + 0.5;
	//	*(input_data2D + width * y + x) = max_of_input;
	//}

	*(input_data2D + width * y1 + x1) = max_of_input;
	*(input_data2D + width * y2 + x2) = max_of_input;
	int x = 3;
	

}

boost::shared_array<double> DrawlineOnImage::GetTestline(int size)
{
	boost::shared_array<double> line(new double[size]);
	
	double dw = 4 * PI / size;
	double phi0 = PI / 4;
	for (int i = 0; i < size; i++)
	{
		*(line.get() + i) =  10 * sin(i * dw + phi0);
	}
	return line;

}