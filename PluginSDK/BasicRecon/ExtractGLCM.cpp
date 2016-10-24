#include "stdafx.h"
#include "ExtractGLCM.h"
#include "Interface/Client/DataHelper.h"
#include <math.h>

using namespace std;
using namespace Yap;
using namespace Yap::Direction;

template<typename T>
void Convert(Yap::DataHelper help, unsigned int * out_data, unsigned int size)
{
	//存储原图中最大值
	T max_value = 0;
	size_t data_size = help.GetDataSize();
	auto in_data = Yap::GetDataArray<T>(help.GetData());
	for (size_t i = 0; i < data_size; ++i)
	{
		max_value = (*in_data >= max_value) ? *in_data : max_value;
		++in_data;
	}
	//指针返回第一个数据
	in_data = Yap::GetDataArray<T>(help.GetData());
	for (size_t i = 0; i < data_size; ++i)
	{
		*out_data = static_cast<unsigned int>( (*in_data) * size / max_value + 0.5);
		++out_data;
		++in_data;
	}
}

Yap::ExtractGLCM::ExtractGLCM():
	ProcessorImpl(L"ExtractGLCM"),
	_glcm_size(256),
	_direction(0),
	_step_size(1)
{
}

Yap::ExtractGLCM::ExtractGLCM(const ExtractGLCM & rh):
	ProcessorImpl(rh)
{
}

Yap::ExtractGLCM::~ExtractGLCM()
{
}

bool Yap::ExtractGLCM::Input(const wchar_t * name, IData * data)
{
	if (std::wstring(name) != L"Input")
		return false;
	if (data == nullptr)
		return false;
	DataHelper data_helper(data);
	
	if (data_helper.GetActualDimensionCount() != 2)
		return false;
	unsigned int glcm_size = GetInt(L"GLCMSize");
	unsigned int direction = GetInt(L"Direction");
	unsigned int step_size = GetInt(L"StepSize");
	TODO(不知道GLCM的大小限制在8~256范围内是否合适)
	if (direction > 12 || direction < 0 || step_size >data_helper.GetWidth() || step_size > data_helper.GetHeight() || step_size < 0 || glcm_size < 8 || glcm_size > 256)
		return false;
	SetGLCMSize(glcm_size);
	SetDirection(direction);
	SetStepSize(step_size);

	auto normalize_data = new unsigned int[data_helper.GetDataSize()];

	Normalization(data, normalize_data);
	
	auto glcm = new unsigned int[(size_t)glcm_size * glcm_size];

	GLCM(normalize_data, glcm, data_helper.GetWidth(), data_helper.GetHeight());

	Dimensions dimensions;
	dimensions(DimensionReadout, 0U, glcm_size)(DimensionPhaseEncoding, 0U, glcm_size);
	auto glcm_shared = Yap::YapShared(new UnsignedIntData(glcm, dimensions, nullptr, true));

	Feed(L"Output", glcm_shared.get());

	return true;
}

Yap::IProcessor * Yap::ExtractGLCM::Clone()
{
	return new(nothrow) ExtractGLCM(*this);
}

bool Yap::ExtractGLCM::OnInit()
{
	AddProperty(PropertyInt, L"GLCMSize", L"The GLCM matrix size(gray level size).");
	AddProperty(PropertyInt, L"Direction", L"The GLCM matrix direction.");
	AddProperty(PropertyInt, L"StepSize", L"The GLCM matrix from derived image step,eg: StepSize = 1, then get image pixels with distance 1.");
	SetInt(L"GLCMSize", 256);
	SetInt(L"Direction", DirectionAll);
	SetInt(L"StepSize", 1);
	//All Type data to Int (0~n) n:8~256
	AddInput(L"Input", 2 , DataTypeAll);
	AddOutput(L"Output", 2 , DataTypeUnsignedInt);
	return true;
}

void Yap::ExtractGLCM::GLCM(unsigned int * input_data, 
	unsigned int * output_data, 
	unsigned int input_width, 
	unsigned int input_height)
{
	auto out_size = GetGLCMSize();
	//init glcm
	for (size_t i = 0; i < out_size * out_size; ++i)
	{
		output_data[i] = 0;
	}
	auto n = GetStepSize();
	assert(n < input_width && n < input_height);
	switch (GetDirection())
	{
		/*
		* 灰度共生矩阵实现不同方向，不同步幅；
		*	r\c	|	-n		0		n
		*----------------------------------
		*	-n	|	l-u-n	u-n		r-u-n
		*	 0	|	l-n		0		r-n
		*	 n	|	l-d-n	d-n		r-d-n
		* 注：l-左，u-上，r-右，d-下，n-步幅为n
		*/
		case DirectionRight:
		{
			int r = 0 * n;
			int c = 1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			//for (unsigned int i = (r < 0 ? 1 : 0); i < input_width - (r > 0 ? -1 : 0); ++i)
			//	{
			//		for (unsigned int j = (c < 0? 1:0); j < input_height + (c > 0? -1:0); ++j)
			//		{
			//			auto row = input_data[(i + r) * input_width + j];
			//			auto column = input_data[i * input_width + j + c];
			//			++output_data[row * out_size + column];
			//		}
			//	}
			break;
		}
		case DirectionDown:
		{
			int r = 1 * n;
			int c = 0 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionLeft:
		{
			int r = 0 * n;
			int c = -1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionUp:
		{
			int r = -1 * n;
			int c = 0 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionRightDown:
		{
			int r = 1 * n;
			int c = 1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionLeftDown:
		{
			int r = 1 * n;
			int c = -1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionLeftUp:
		{
			int r = -1 * n;
			int c = -1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionRightUp:
		{
			int r = -1 * n;
			int c = 1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionRightAndDown:
		{
			int r = 0 * n;
			int c = 1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = 1 * n;
			c = 0 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionLeftAndDown:
		{
			int r = 0 * n;
			int c = -1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = 1 * n;
			c = 0 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionLeftAndUp:
		{
			int r = 0 * n;
			int c = -1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = -1 * n;
			c = 0 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionRightAndUp:
		{
			int r = 0 * n;
			int c = 1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = -1 * n;
			c = 0 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		case DirectionAll:
		{
			//实现上、下、左、右、右下、左下、左上、右上，四个方向。
			int r = 0 * n;
			int c = 1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = 1 * n;
			c = 0 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = 0 * n;
			c = -1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = -1 * n;
			c = 0 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = 1 * n;
			c = 1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = 1 * n;
			c = -1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = -1 * n;
			c = -1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			r = -1 * n;
			c = 1 * n;
			AddDirection(input_data, output_data, input_width, input_height, out_size, r, c);
			break;
		}
		default:
			return ;
	}
}

void Yap::ExtractGLCM::Normalization(IData * data, unsigned int * out)
{
	auto size = GetGLCMSize();
	assert(size <= 256);
	DataHelper help(data);

	int type = data->GetDataType();
	switch (type)
	{
		case DataTypeComplexFloat:
			{
				//存储原图中最大值
				double max_value = 0;
				size_t data_size = help.GetDataSize();
				auto in_data = Yap::GetDataArray<std::complex<float>>(help.GetData());
				//search max
				for (size_t i = 0; i < data_size; ++i)
				{
					auto mo = sqrt(in_data->real() * in_data->real() + in_data->imag() * in_data->imag());
					max_value = (mo >= max_value) ? mo : max_value;
					++in_data;
				}
				//指针返回第一个数据
				in_data = Yap::GetDataArray<std::complex<float>>(help.GetData());
				for (size_t i = 0; i < data_size; ++i)
				{
					auto sq = sqrt(in_data->real() * in_data->real() + in_data->imag() * in_data->imag());
					*out = static_cast<unsigned int>(sq * size / max_value + 0.5);
					++out;
					++in_data;
				}
				break;
			}
		case DataTypeComplexDouble:
			{
				//存储原图中最大值
				double max_value = 0;
				size_t data_size = help.GetDataSize();
				auto in_data = Yap::GetDataArray<std::complex<double>>(help.GetData());
				//search max value
				for (size_t i = 0; i < data_size; ++i)
				{
					auto mo = sqrt(in_data->real() * in_data->real() + in_data->imag() * in_data->imag());
					max_value = (mo >= max_value) ? mo : max_value;
					++in_data;
				}
				//指针返回第一个数据
				in_data = Yap::GetDataArray<std::complex<double>>(help.GetData());
				for (size_t i = 0; i < data_size; ++i)
				{
					auto sq = sqrt(in_data->real() * in_data->real() + in_data->imag() * in_data->imag());
					*out = static_cast<unsigned int>(sq * size / max_value + 0.5);
					++out;
					++in_data;
				}
				break;
			}
		case DataTypeUnsignedInt:
			Convert<unsigned int>(help, out, size);
			break;
		case DataTypeUnsignedShort:
			Convert<unsigned short>(help, out, size);
			break;
		case DataTypeInt:
			Convert<int>(help, out, size);
			break;
		case DataTypeShort:
			Convert<short>(help, out, size);
			break;
		case DataTypeDouble:
			Convert<double>(help, out, size);
			break;
		case DataTypeFloat:
			Convert<float>(help, out, size);
			break;
		default:
			return;
	}

}

void Yap::ExtractGLCM::SetGLCMSize(unsigned int glcm_size)
{
	_glcm_size = glcm_size;
}

unsigned int Yap::ExtractGLCM::GetGLCMSize()
{
	return _glcm_size;
}

void Yap::ExtractGLCM::SetDirection(unsigned int direction)
{
	_direction = direction;
}

unsigned int Yap::ExtractGLCM::GetDirection()
{
	return _direction;
}

void Yap::ExtractGLCM::SetStepSize(unsigned int step_size)
{
	_step_size = step_size;
}

unsigned int Yap::ExtractGLCM::GetStepSize()
{
	return _step_size;
}

/*
*在求GLCM的时候需要判断不同方向对原图像矩阵横纵边界的影响：
*	step_r		i		width
*--------------------------------
*		 n		0			-n
*		 0		0			 0
*		-n		n			 0
*so: unsigned int step_r = (step_r < 0 ? n : 0); and row = ***[(i + step_r)***]***
*
*	step_c		j		height
*-------------------------------
*		 n		0			-n
*		 0		0			 0
*		-n		n			 0
*so: unsigned int step_c = (step_c < 0 ? n : 0); and column = ***[*** + j + step_c];
*/
void Yap::ExtractGLCM::AddDirection(unsigned int * input_data, 
	unsigned int * output_data, 
	unsigned int input_width, 
	unsigned int input_height, 
	unsigned int out_size, 
	int step_row, 
	int step_column)
{
	for (unsigned int i = (step_row < 0 ? -step_row : 0); i < input_width + (step_row > 0 ? -step_row : 0); ++i)
	{
		for (unsigned int j = (step_column < 0 ? -step_column : 0); j < input_height + (step_column > 0 ? -step_column : 0); ++j)
		{
			auto row = input_data[i * input_width + j];
			auto column = input_data[(i + step_row) * input_width + j + step_column];
			++output_data[row * out_size + column];
		}
	}
}