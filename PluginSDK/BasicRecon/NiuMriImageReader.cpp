#include "stdafx.h"
#include "NiuMriImageReader.h"
#include "Implement/LogUserImpl.h"

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
		struct NiuMriImageFileHeaderInfo
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

NiuMriImageReader::NiuMriImageReader(void) :
	ProcessorImpl(L"NiuMriImageReader")
{
	LOG_TRACE(L"NiuMriImageReader constructor called.", L"BasicRecon");
	AddInput(L"Input", 0, DataTypeUnknown);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeUnsignedShort);

	AddProperty<const wchar_t * const>(L"DataPath", L"", L"数据文件夹和文件名。");
}

Yap::NiuMriImageReader::NiuMriImageReader(const NiuMriImageReader& rhs) :
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"NiuMriImageReader copy constructor called.", L"BasicRecon");
}

Yap::NiuMriImageReader::~NiuMriImageReader()
{
}

bool Yap::NiuMriImageReader::Input(const wchar_t * name, IData * data)
{
	// Should not pass in data to start raw data file reading.
	assert(data == nullptr);

	if (!ReadNiuMriImageData())
		return false;

	return true;
}

bool Yap::NiuMriImageReader::ReadNiuMriImageData()
{
	std::wostringstream output(GetProperty<const wchar_t * const>(L"DataPath"));
	wstring data_path = output.str();

	try
	{
		ifstream file(data_path.c_str(), ios::binary);

		//read image data header
		details::NiuMriImageFileHeaderInfo sections;
		if (!file.read(reinterpret_cast<char*>(&sections), sizeof(details::NiuMriImageFileHeaderInfo)))
			return false;

		int section6_offset = sizeof(details::NiuMriImageFileHeaderInfo) +
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
		if (dim1 > 2048 || dim2 > 2048 || dim3 > 1024	)
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

		auto data = CreateData<unsigned short>(nullptr,
			reinterpret_cast<unsigned short*>(buffer), dimensions);

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
