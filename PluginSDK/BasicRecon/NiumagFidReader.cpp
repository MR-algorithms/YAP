#include "stdafx.h"
#include "NiumagFidReader.h"
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
		struct NiumagFidFileHeaderInfo
		{
			int FileVersion;
			int Section1Size;
			int Section2Size;
			int Section3Size;
			int Section4Size;
			int Section5Size;
		};
	}
}

NiumagFidReader::NiumagFidReader(void):
	ProcessorImpl(L"NiumagFidReader")
{
	LOG_TRACE(L"NiumagFidReader constructor called.", L"BasicRecon");
	AddInput(L"Input", 0, DataTypeUnknown);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexFloat);

	AddProperty<const wchar_t * const>(L"DataPath", L"", L"数据文件夹和文件名。");
}

NiumagFidReader::NiumagFidReader(const NiumagFidReader& rhs):
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"NiumagFidReader constructor called.", L"BasicRecon");
}

NiumagFidReader::~NiumagFidReader()
{
	LOG_TRACE(L"NiumagFidReader destructor called.", L"BasicRecon");
}

bool Yap::NiumagFidReader::Input(const wchar_t * name, IData * data)
{
	// Should not pass in data to start raw data file reading.
	assert(data == nullptr);

	if (!ReadNiumagFidData())
		return false;

	return true;
}

bool Yap::NiumagFidReader::ReadNiumagFidData()
{
	std::wostringstream output(GetProperty<const wchar_t * const>(L"DataPath"));
	wstring data_path = output.str();

	try
	{
		ifstream file(data_path.c_str(), ios::binary);

		details::NiumagFidFileHeaderInfo sections;
		if (!file.read(reinterpret_cast<char*>(&sections), sizeof(details::NiumagFidFileHeaderInfo)))
			return false;

		int section5_offset = sizeof(details::NiumagFidFileHeaderInfo) +
			sections.Section1Size +
			sections.Section2Size +
			sections.Section3Size +
			sections.Section4Size;

		file.seekg(section5_offset, ios::beg);

		int buf[4];
		file.read(reinterpret_cast<char*>(buf), sizeof(int) * 4);
		int dim1 = buf[0];
		int dim2 = buf[1];
		int dim3 = buf[2];
		int dim4 = buf[3];

		if (dim1 > 8192 || dim2 > 8192 || dim3 > 2048 || dim4 > 2048)
		{
			throw std::ifstream::failure("out of range");
		}

		unsigned buffer_size = dim1 * dim2 * dim3 * dim4 * 2;
		float * buffer = new float[buffer_size];

		if (!file.read(reinterpret_cast<char*>(buffer), buffer_size * sizeof(float)))
		{
			delete[]buffer;
			return false;
		}

		Dimensions dimensions;
		dimensions(DimensionReadout, 0U, dim1)
			(DimensionPhaseEncoding, 0U, dim2)
			(DimensionSlice, 0U, dim3)
			(Dimension4, 0U, dim4);

		auto data = CreateData<complex<float>>(
			reinterpret_cast<complex<float>*>(buffer), dimensions, nullptr, true);

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
