#include "stdafx.h"
#include "CmrDataReader.h"

#include <sstream>
#include <iostream>
#include <boost\assign\list_of.hpp>
#include "..\Interface\SmartPtr.h"
#include <iomanip>
#include <fstream>

using namespace Yap;
using namespace std;
using boost::assign::list_of;


CCmrDataReader::CCmrDataReader(void) :
	CProcessorImp(L"CmrRawDataReader")
{
}

CCmrDataReader::CCmrDataReader(const CCmrDataReader& rhs)
	: CProcessorImp(rhs)
{
	AddInputPort(L"Input", 0, DataTypeUnknown);
	AddOutputPort(L"Output", 0, DataTypeComplexFloat);

	AddProperty(PropertyString, L"DataPath",  L"包含原始数据文件的文件夹。");
	AddProperty(PropertyInt, L"ChannelCount",  L"通道数");
	AddProperty(PropertyInt, L"ChannelSwitch",  L"通道开关指示值");
	AddProperty(PropertyInt, L"GroupCount",  L"分组扫描数");
}

IProcessor * CCmrDataReader::Clone()
{
	try
	{
		auto processor = new CCmrDataReader(*this);
		return processor;
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}

bool CCmrDataReader::Input(const wchar_t * name, IData * data)
{
	// Should not pass in data to start raw data file reading.
	assert(data == nullptr);

	int channel_count = GetInt(L"ChannelCount");
	assert(channel_count > 0 && channel_count <= 32);

	for (int channel_index = 0; channel_index < channel_count; ++channel_index)
	{
		unsigned int channel_mask = (1 << channel_index); // 每次循环都和0或，得到某通道0001(1),0010(2),0100(4),1000(8)
		bool channel_used = ((channel_mask & GetInt(L"ChannelSwitch")) == channel_mask);    // channel_mask只要和给的通道一样，就必定等于channel_mask自己
		if (channel_used)
		{
			if (!ReadRawData(channel_index))
			{
				return false;
			}
		}
	}

	return true;
}

bool CCmrDataReader::ReadRawData(unsigned int channel_index)
{
	std::wostringstream output;
	output << GetString(L"DataPath") << L"\\ChannelData"
		<< setfill(L'0') << setw(2) << channel_index + 1 << L".fid";

	std::vector<float*> channel_data_buffer;
	std::vector<unsigned int> slices;
	unsigned int width, height, dim4, total_slice_count = 0; // 对于未进行累加处理的数据，得到的slice实际是 真实的slice  × 实际累加次数。
	int group_count = GetInt(L"GroupCount");
	if (group_count == 0)
	{
		group_count = 1;
	}

	for (int i = 0; i < group_count; ++i)
	{
		wstring data_path = output.str();
		if (group_count > 1)
		{
			wostringstream temp_output;
			temp_output << L"." << group_count << L"." << i + 1;

			data_path += temp_output.str();
		}

		unsigned int group_slice_count = 0;
		float * temp_raw_data_buffer = ReadEcnuFile(data_path.c_str(), width, height, group_slice_count, dim4);
		if (temp_raw_data_buffer == nullptr)
		{
			return false;
		}
		channel_data_buffer.push_back(temp_raw_data_buffer);
		total_slice_count += group_slice_count;
		slices.push_back(group_slice_count);
	}

	float* raw_data_buffer = nullptr;
	if (channel_data_buffer.size() == 1)
	{
		raw_data_buffer = channel_data_buffer[0];
	}
	else
	{
		try
		{
			raw_data_buffer = new float[width * height * total_slice_count * 2];
		}
		catch(bad_alloc&)
		{
			return false;
		}

		unsigned int offset = 0;
		for (unsigned int i = 0; i < channel_data_buffer.size(); ++i)
		{
			std::memcpy(raw_data_buffer + offset, channel_data_buffer[i], width * height * slices[i] * 2 * sizeof(float));
			offset += width * height * slices[i] * 2;
		}
	}
	

	CDimensionsImp dimensions;
	dimensions(DimensionReadout, 0U, width)
			  (DimensionPhaseEncoding, 0U, height)
			  (DimensionSlice, 0U, total_slice_count)
			  (Dimension4, 0U, dim4)
			  (DimensionChannel, channel_index, 1);

	CSmartPtr<CComplexFloat> data (new CComplexFloat(
		reinterpret_cast<complex<float>*>(raw_data_buffer), dimensions, nullptr, true));

	Feed(L"Output", data.get());

	return true;
}

/// This structure is used to store version info and sizes of all sections.
struct EcnuRawSections
{
	double  FileVersion;
	int  Section1Size;
	int  Section2Size;
	int  Section3Size;
	int  Section4Size;
};

/**
Read raw data from raw data file.
@return Pointer to the buffer used to store the raw-data. Should be released by the caller of this function.
@param width Output parameter used to store the width of the image (in pixel).
@param height Output, height of the image.
@param slices Output, number of slices read.
*/
float* CCmrDataReader::ReadEcnuFile(const wchar_t * file_path,
	unsigned int& width,
	unsigned int& height,
	unsigned int& slices,
	unsigned int& dim4)
{
	ifstream file(file_path, ios::binary);

	// read raw data header
	EcnuRawSections sections;
	if (!file.read(reinterpret_cast<char*>(&sections), sizeof(EcnuRawSections)))
		return nullptr;

	// read dimension information
	file.seekg(sizeof(EcnuRawSections) + sections.Section1Size + sections.Section2Size + sections.Section3Size,
		ios::beg);

	// Version 1.5701001以上版本, 允许浮点误差
	if (sections.FileVersion - 1.5701 > 0.00000005)
	{
		int buf[5];
		file.read(reinterpret_cast<char*>(buf), sizeof(int) * 5);
		assert(buf[4] == 1);
		width = buf[0];
		height = buf[1];
		slices = buf[2] * buf[3];
		dim4 = buf[3];
	}
	else
	{
		// 1.5702版本以上的谱仪版本，数据增加到5维，目前暂时第5维为1.
		int buf[4];
		file.read(reinterpret_cast<char*>(buf), sizeof(int) * 4);
		width = buf[0];
		height = buf[1];
		slices = buf[2] * buf[3];
		dim4 = buf[3];
	}

	unsigned buffer_size = width * height * 2 * slices;
	float * data = new float[buffer_size];

	// read actual data
	if (file.read(reinterpret_cast<char*>(data), buffer_size * sizeof(float)))
	{
		return data;
	}
	else
	{
		delete[]data;
		return nullptr;
	}
}
