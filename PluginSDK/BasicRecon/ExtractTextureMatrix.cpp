#include "stdafx.h"
#include "ExtractTextureMatrix.h"
#include "Interface/Client/DataHelper.h"
#include <math.h>
#include <map>
#include <tuple>

using namespace std;
using namespace Yap;
using namespace Yap::Direction;

static map<std::wstring, unsigned int> _direction_map = {
	{ L"DirectionRight", Direction::DirectionRight },
	{ L"DirectionDown", Direction::DirectionDown },
	{ L"DirectionLeft",Direction::DirectionLeft },
	{ L"DirectionUp", Direction::DirectionUp },
	{ L"DirectionRightDown", Direction::DirectionRightDown },
	{ L"DirectionLeftDown", Direction::DirectionLeftDown },
	{ L"DirectionLeftUp", Direction::DirectionLeftUp },
	{ L"DirectionRightUp", Direction::DirectionRightUp },
	{ L"DirectionRightAndDown", Direction::DirectionRightAndDown },
	{ L"DirectionLeftAndDown", Direction::DirectionLeftAndDown },
	{ L"DirectionLeftAndUp", Direction::DirectionLeftAndUp },
	{ L"DirectionRightAndUp", Direction::DirectionRightAndUp },
	{ L"DirectionAll", Direction::DirectionAll }
};

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

Yap::ExtractTextureMatrix::ExtractTextureMatrix():
	ProcessorImpl(L"ExtractTextureMatrix"),
	_gray_level(256),
	_glszm_size(256),
	_direction(0),
	_step_size(1)
{
	AddProperty(PropertyInt, L"GrayLevel", L"The GLCM matrix size(gray level size).");
	AddProperty(PropertyString, L"Direction", L"The GLCM matrix direction.");
	AddProperty(PropertyInt, L"StepSize", L"The GLCM matrix from derived image step, eg: StepSize = 1, then get image pixels with distance 1.");
	AddProperty(PropertyInt, L"GLSZMSize", L"The GLSZM matrix size");
	SetInt(L"GrayLevel", 256);
	SetInt(L"GLSZMSize", 256);
	SetString(L"Direction", L"DirectionRight");
	SetInt(L"StepSize", 1);
	//All Type data to Int (0~n) n:8~256
	AddInput(L"Input", 2, DataTypeAll);
	AddOutput(L"GLCM", 1, DataTypeFloat);
	AddOutput(L"GLSZM", 2, DataTypeFloat);
}

Yap::ExtractTextureMatrix::~ExtractTextureMatrix()
{
}

bool Yap::ExtractTextureMatrix::Input(const wchar_t * name, IData * data)
{
	if (std::wstring(name) != L"Input")
		return false;
	if (data == nullptr)
		return false;
	DataHelper data_helper(data);
	
	if (data_helper.GetActualDimensionCount() != 2)
		return false;
	unsigned int gray_level= GetInt(L"GrayLevel");
	unsigned int glszm_size = GetInt(L"GLSZMSize");
	wstring direction = GetString(L"Direction");
	unsigned int step_size = GetInt(L"StepSize");

	TODO(不知道gray_level的大小限制在8~256范围内是否合适)
	if (GetDirectionFromName(direction) < 0)
			return false;
	if (glszm_size > data_helper.GetWidth() ||
		glszm_size > data_helper.GetHeight()|| 
		step_size >data_helper.GetWidth() || 
		step_size > data_helper.GetHeight() || 
		step_size < 0 || 
		gray_level < 8 ||
		gray_level > 256)
		return false;
	SetGLSZMSize(glszm_size);
	SetGrayLevel(gray_level);
	SetDirection(GetDirectionFromName(direction));
	SetStepSize(step_size);

	auto normalize_data = new unsigned int[data_helper.GetDataSize()];

	Normalization(data, normalize_data);

	if (OutportLinked(L"GLCM") || OutportLinked(L"Output"))
	{
		auto glcm = new unsigned int[(size_t)_gray_level * _gray_level];

		GLCM(normalize_data, glcm, data_helper.GetWidth(), data_helper.GetHeight());

		
		//glcm_index: size-5: 0: asm, 1: contrast, 2: IDM, 3: entropy, 4: corration.
		//glcm_percent: 矩阵的概率形式
		float glcm_index[(size_t)5];

		ExtractGLCMFeature(glcm, glcm_index);

		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, 5)(DimensionPhaseEncoding, 0U, 5);
		auto glcm_shared = Yap::YapShared(new FloatData(glcm_index, dimensions, nullptr, true));

		Feed(L"GLCM", glcm_shared.get());

		return true;

	}
	
	if (OutportLinked(L"GLSZM"))
	{
		//The size of glszm is row=gray_level and column=glszm_size
		auto glszm = new unsigned int[(size_t)gray_level * glszm_size];

		GLSZM(normalize_data, glszm, data_helper.GetWidth(), data_helper.GetHeight());


		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, glszm_size)(DimensionPhaseEncoding, 0U, glszm_size);
		auto glszm_shared = Yap::YapShared(new UnsignedIntData(glszm, dimensions, nullptr, true));

		Feed(L"GLSZM", glszm_shared.get());

		return true;

	}
}

Yap::IProcessor * Yap::ExtractTextureMatrix::Clone()
{
	return new(nothrow) ExtractTextureMatrix(*this);
}

void Yap::ExtractTextureMatrix::GLCM(unsigned int * input_data, 
	unsigned int * output_data, 
	unsigned int input_width, 
	unsigned int input_height)
{
	auto out_size = GetGrayLevel();
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

void Yap::ExtractTextureMatrix::GLSZM(unsigned int * input_data, unsigned int * output_data, unsigned int input_width, unsigned int input_height)
{
	auto out_row = GetGrayLevel();
	auto out_column = GetGLSZMSize();
	for (size_t i = 0; i < out_row * out_column; ++i)
	{
		output_data[i] = 0;
	}
	//记录一次zone size的个数
	std::map<std::tuple<unsigned int, unsigned int>, unsigned int> zone;
	//外部循环是为了得到不同灰度的区域，内部循环是为了得到相同灰度区域的大小
	for (unsigned int row = 0; row < input_width ; ++row)
	{
		for (unsigned int column = 0; column < input_height ; ++column)
		{
			TODO(还未实现GLSZM部分的代码)
		}
	}
		
}
void Yap::ExtractTextureMatrix::Normalization(IData * data, unsigned int * out)
{
	auto size = GetGrayLevel();
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

void Yap::ExtractTextureMatrix::ExtractGLCMFeature(unsigned int * input_data, float * output_data)
{
	auto glcm_percent = new float[(size_t)_gray_level * _gray_level];
	auto m = 0;

	for (int i = 0; i < _gray_level * _gray_level; ++i)
		m += input_data[i];

	for (int i = 0; i < _gray_level * _gray_level; ++i)
		glcm_percent[i] /= (float)input_data[i] / m;
	//x方向的平均值： u(i) = sum sum i*p(i,j)
	//y方向的平均值： u(j) = sum sum j*p(i,j)
	//x方向的均方差：s(i)^2 = sum sum p(i,j) * (i - u(i))^2
	//y方向的均方差：s(j)^2 = sum sum p(i,j) * (j - u(j))^2
	float u_x = 0.0, u_y = 0.0, s_x = 0.0, s_y = 0.0;
	//u_x,u_y,s_x,s_y的计算公式不确定是否正确
	for (unsigned int i = 0; i < _gray_level; ++i)
	{
		for (unsigned int j = 0; j < _gray_level; ++j)
		{
			u_x += (i + 1) * glcm_percent[i * _gray_level + j];
			u_y += (j + 1) * glcm_percent[i * _gray_level + j];
		}
	}

	for (unsigned int i = 0; i < _gray_level; ++i)
	{
		for (unsigned int j = 0; j < _gray_level; ++j)
		{
			s_x += glcm_percent[i * _gray_level + j] * (i - u_x) * (i - u_x);
			s_y += glcm_percent[i * _gray_level + j] * (j - u_y) * (j - u_y);
		}
	}
	s_x = sqrt(s_x);
	s_y = sqrt(s_y);
	auto u_xy = u_x * u_y;
	auto s_xy = s_x * s_y;

	//output_data[0]: asm 能量 sum sum p^2
	//output_data[1]: contrast:反差/对比： (i-j)^2 * P(i,j)
	//output_data[2]: IDM: (逆差矩，局部平稳)： sum(sum(p(i,j)/(1 + (i-j)^2)))
	//output_data[3]: Entropy:(熵) ： sum(sum( p(i,j) * ln(p(i,j)) ))
	//output_data[4]: correlation相关性: sum(sum( (i*j*p(i,j)*-u(i)*u(j)) / s(i)*s(j) ))
	output_data[0] = 0.0;
	output_data[1] = 0.0;
	output_data[2] = 0.0;
	output_data[3] = 0.0;
	output_data[4] = 0.0;
	for (unsigned int i = 0; i < _gray_level; ++i)
	{
		for (unsigned int j = 0; j < _gray_level; ++j)
		{
			output_data[0] += glcm_percent[i * _gray_level + j] * glcm_percent[i * _gray_level + j];
			output_data[1] += (i - j) * (i - j) * glcm_percent[i * _gray_level + j];
			output_data[2] += glcm_percent[i *_gray_level + j] / (1 + (i - j)* (i - j));
			output_data[3] += -glcm_percent[i *_gray_level + j] * log(glcm_percent[i *_gray_level + j]);
			output_data[4] += (i * j * glcm_percent[i * _gray_level + j] - u_xy) / s_xy;
		}
	}
}

void Yap::ExtractTextureMatrix::SetGLSZMSize(unsigned int glszm_size)
{
	_glszm_size = glszm_size;
}

unsigned int Yap::ExtractTextureMatrix::GetGLSZMSize()
{
	return _glszm_size;
}

void Yap::ExtractTextureMatrix::SetGrayLevel(unsigned int glcm_size)
{
	_gray_level = glcm_size;
}

unsigned int Yap::ExtractTextureMatrix::GetGrayLevel()
{
	return _gray_level;
}

void Yap::ExtractTextureMatrix::SetDirection(unsigned int direction)
{
	_direction = direction;
}

unsigned int Yap::ExtractTextureMatrix::GetDirection()
{
	return _direction;
}

void Yap::ExtractTextureMatrix::SetStepSize(unsigned int step_size)
{
	_step_size = step_size;
}

unsigned int Yap::ExtractTextureMatrix::GetStepSize()
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
void Yap::ExtractTextureMatrix::AddDirection(unsigned int * input_data, 
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

int Yap::ExtractTextureMatrix::GetDirectionFromName(wstring name)
{
	auto direction = _direction_map.find(name);
	if (direction != _direction_map.end())
	{
		return direction->second;
	}
	return -1;
}
