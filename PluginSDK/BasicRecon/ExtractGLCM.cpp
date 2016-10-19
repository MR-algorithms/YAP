#include "stdafx.h"
#include "ExtractGLCM.h"
#include "Interface/Client/DataHelper.h"
#include <math.h>


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
	ProcessorImpl(L"ExtractGLCM")
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
	unsigned int size = GetInt(L"GLCMSize");
	assert(size >= 8 && size <= 256);

	auto normalize_data = new unsigned int[data_helper.GetDataSize()];

	Normalization(data, normalize_data, size);
	
	auto glcm = new unsigned int[(size_t)size * size];

	GLCM(normalize_data, glcm, data_helper.GetWidth(), data_helper.GetHeight(), size);

	Dimensions dimensions;
	dimensions(DimensionReadout, 0U, size)(DimensionPhaseEncoding, 0U, size);
	auto glcm_shared = Yap::YapShared(new UnsignedIntData(glcm, dimensions, nullptr, true));

	Feed(L"Output", glcm_shared.get());

	return true;
}

Yap::IProcessor * Yap::ExtractGLCM::Clone()
{
	TODO(new没有加nothrow，加nothrow后就会报错)
	return new ExtractGLCM(*this);
}

bool Yap::ExtractGLCM::OnInit()
{
	AddProperty(PropertyInt, L"GLCMSize", L"The GLCM matrix size(gray level size).");
	SetInt(L"GLCMSize", 256);
	//All Type data to Int (0~n) n:8~255
	AddInput(L"Input", 2 , DataTypeAll);
	AddOutput(L"Output", 2 , DataTypeUnsignedInt);
	return true;
}

void Yap::ExtractGLCM::GLCM(unsigned int * input_data, unsigned int * output_data, unsigned int input_width, unsigned int input_height, unsigned int out_size)
{

	//init glcm
	for (size_t i = 0; i < out_size * out_size; ++i)
	{
		output_data[i] = 0;
	}
	//仅仅实现横向GLCM，row不变，column + 1
	for (size_t i = 0; i < input_width; ++i)
	{
		for (size_t j = 0; j < input_height - 1; ++j)
		{
			auto row = input_data[i * input_width + j];
			auto column = input_data[i * input_width + j + 1];
			++output_data[row * (size_t)out_size + column];
		}
	}
}

void Yap::ExtractGLCM::Normalization(IData * data, unsigned int * out, unsigned int size)
{
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
