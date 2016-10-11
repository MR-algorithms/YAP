#include "stdafx.h"
#include "NiumagImgReader2.h"

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

NiumagImgReader2::NiumagImgReader2(void) :
	ProcessorImpl(L"NiumagImgReader2")
{
}

Yap::NiumagImgReader2::NiumagImgReader2(const NiumagImgReader2& rhs) :
	ProcessorImpl(rhs)
{
}

bool Yap::NiumagImgReader2::Input(const wchar_t * name, IData * data)
{
	// Should not pass in data to start raw data file reading.
	assert(data == nullptr);

	if (!ReadNiumagImgData2())
		return false;

	return true;
}

IProcessor * Yap::NiumagImgReader2::Clone()
{
	return new(nothrow) NiumagImgReader2(*this);
}

bool Yap::NiumagImgReader2::OnInit()
{
	AddInput(L"Input", 0, DataTypeUnknown);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeUnsignedShort);

	AddProperty(PropertyString, L"DataPath", L"数据文件夹和文件名。");

	return true;
}

bool Yap::NiumagImgReader2::ReadNiumagImgData2()
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

		int buf[2];
		file.read(reinterpret_cast<char*>(buf), sizeof(int) * 2);
		int dim1 = buf[0];
		int dim2 = buf[1];

		assert(sizeof(unsigned short) == 2);
		if (dim1 > 2048 || dim2 > 2048)
		{
			throw std::ifstream::failure("out of range");
		}

		unsigned buffer_size = dim1 * dim2;
		unsigned short * buffer = new unsigned short[buffer_size];

		if (!file.read(reinterpret_cast<char*>(buffer), buffer_size * sizeof(unsigned short)))
		{
			delete[]buffer;
			return false;
		}

		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, dim1)
			(DimensionPhaseEncoding, 0U, dim2);

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
