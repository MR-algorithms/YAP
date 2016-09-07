#include "stdafx.h"
#include "NiumagImgReader.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace Yap;
using namespace std;

namespace Yap
{
	namespace details
	{

		/// This structure is used to store version info and sizes of all sections.
		struct NiumagImgFileHeaderInfo
		{
			int  FileVersion;
			int  Section1Size;
			int  Section2Size;
			int  Section3Size;
			int  Section4Size;
			int  Section5Size;
			int  Section6Size;
			int  Section7Size;
			int  Section8Size;
		};

	}
}

CNiumagImgReader::CNiumagImgReader():
	ProcessorImpl(L"NiumagImgReader")
{
}

Yap::CNiumagImgReader::CNiumagImgReader(const CNiumagImgReader& rhs):
	ProcessorImpl(rhs)
{
}

bool Yap::CNiumagImgReader::Input(const wchar_t * name, IData * data)
{
	// Should not pass in data to start raw data file reading.
	assert(data == nullptr);

	if (!ReadNiumagImgData())
		return false;

	return true;
}

IProcessor * Yap::CNiumagImgReader::Clone()
{
	return new(nothrow) CNiumagImgReader(*this);
}

bool Yap::CNiumagImgReader::OnInit()
{
	AddInput(L"Input", 0, DataTypeUnknown);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeUnsignedShort);

	AddProperty(PropertyString, L"DataPath", L"�����ļ��к��ļ�����");

	return true;
}

bool Yap::CNiumagImgReader::ReadNiumagImgData()
{
	std::wostringstream output(GetString(L"DataPath"));
	wstring data_path = output.str();

	try
	{
		ifstream file(data_path.c_str(), ios::binary);

		//read image data header
		details::NiumagImgFileHeaderInfo sections;
		if (!file.read(reinterpret_cast<char*>(&sections), sizeof(details::NiumagImgFileHeaderInfo)))
			return false;

		//
		int section6_offset = sizeof(details::NiumagImgFileHeaderInfo) +
			sections.Section1Size +
			sections.Section2Size +
			sections.Section3Size +
			sections.Section4Size +
			sections.Section5Size;

		file.seekg(section6_offset, ios::beg);

		int buf[3];
		file.read(reinterpret_cast<char*>(buf), sizeof(int) * 3);
		int dim1 = buf[0];
		int dim2 = buf[1];
		int dim3 = buf[2];

		assert(sizeof(unsigned short) == 2);
		if (dim1 > 2048 || dim2 > 2048 || dim3 > 1024)
		{
			throw std::ifstream::failure("out of range");
		}

		unsigned buffer_size = dim1 * dim2 * dim3;
		unsigned short * buffer = new unsigned short[buffer_size];

		if (!file.read(reinterpret_cast<char*>(buffer), buffer_size * sizeof(unsigned short)))
		{
			delete[]buffer;
			return false;
		}

		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, dim1)
			(DimensionPhaseEncoding, 0U, dim2)
			(DimensionSlice, 0U, dim3);

		auto data = YapShared(new UnsignedShortData(
			reinterpret_cast<unsigned short*>(buffer), dimensions, nullptr, true));

		Feed(L"Output", data.get());
	}
	catch (std::ifstream::failure&)
	{
		return false;
	}
	catch (bad_alloc&)
	{
		return false;
	}
	
	return true;
}
