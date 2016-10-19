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
	unsigned int size = GetBool(L"GLCMSize");
	assert(size >= 8 && size <= 256);

	//auto out_data = Yap::YapShared(new DataObject<unsigned int>(data->GetDimensions()));
	auto output = Normalization(data, size);
	
	auto glcm = GLCM(output.get(),size);
	Feed(L"Output", glcm);
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

Yap::IData * Yap::ExtractGLCM::GLCM(IData * data,unsigned int size)
{
	assert(data != nullptr);
	DataHelper help(data);
	auto image = new DataObject<unsigned int>(data->GetDimensions());
	auto image_data = image->GetData();
	auto glcm = new unsigned int[size * size];
	for (unsigned int i = 0; i < size * size; ++i)
	{
		glcm[i] = 0;
	}
	//仅仅实现横向GLCM，row不变，column + 1
	for (size_t i = 0; i < help.GetWidth(); ++i)
	{
		for (size_t j = 0; j < help.GetHeight() - 1; ++j)
		{
			++glcm[image_data[i * help.GetWidth() + j] * size + image_data[i * help.GetWidth() + j + 1]];
		}
	}

	Dimensions dimensions;
	dimensions(DimensionReadout, 0U, size)(DimensionPhaseEncoding, 0U, size);
	auto glcm_shared = Yap::YapShared(new DataObject<unsigned int>(glcm, dimensions, nullptr, true));

	return glcm_shared.get();
}

Yap::SmartPtr<Yap::IData> Yap::ExtractGLCM::Normalization(IData * data, unsigned int size)
{
	assert(size <= 256);
	DataHelper help(data);
	//归一化的数据对象
	auto output = Yap::YapShared(new DataObject<unsigned int>(data->GetDimensions()));
	//获得归一化数组的指针
	auto out_data = Yap::GetDataArray<unsigned int>(output.get());

	int type = data->GetDataType();
	switch (type)
	{
		case DataTypeComplexFloat:
			{
				//存储原图中最大值
				double max_value = 0;
				size_t data_size = help.GetDataSize();
				auto in_data = Yap::GetDataArray<std::complex<float>>(help.GetData());
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
					*(out_data) = static_cast<unsigned int>(sq * size / max_value + 0.5);
					++out_data;
					++in_data;
				}

				return output;
			}
		case DataTypeComplexDouble:
			{
				//存储原图中最大值
				double max_value = 0;
				size_t data_size = help.GetDataSize();
				auto in_data = Yap::GetDataArray<std::complex<double>>(help.GetData());
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
					*(out_data) = static_cast<unsigned int>(sq * size / max_value + 0.5);
					++out_data;
					++in_data;
				}

				return output;
			}
		case DataTypeUnsignedInt:
			Convert<unsigned int>(help, out_data, size);
			return output;
		case DataTypeUnsignedShort:
			Convert<unsigned short>(help, out_data, size);
			return output;
		case DataTypeInt:
			Convert<int>(help, out_data, size);
			return output;
		case DataTypeShort:
			Convert<short>(help, out_data, size);
			return output;
		case DataTypeDouble:
			Convert<double>(help, out_data, size);
			return output;
		case DataTypeFloat:
			Convert<float>(help, out_data, size);
			return output;
		default:
			return Yap::SmartPtr<IData>();
	}
	return Yap::SmartPtr<IData>();
}
