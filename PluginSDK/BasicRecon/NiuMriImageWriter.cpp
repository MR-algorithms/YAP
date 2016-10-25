#include "stdafx.h"
#include "NiuMriImageWriter.h"

#include <fstream>
#include <iosfwd>
#include "Interface\Client\DataHelper.h"

using namespace Yap;
using namespace std;

NiuMriImageWriter::NiuMriImageWriter(void) :
	ProcessorImpl(L"NiuMriImageWriter")
{
	AddInput(L"Input", 2, DataTypeUnsignedShort);
	AddProperty(PropertyString, L"ExportFolder", L"Set folder used to write images.");
}

NiuMriImageWriter::NiuMriImageWriter(const NiuMriImageWriter& rhs) :
	ProcessorImpl(rhs)
{
}

IProcessor * Yap::NiuMriImageWriter::Clone()
{
	return new(nothrow) NiuMriImageWriter(*this);
}

bool Yap::NiuMriImageWriter::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && (GetDataArray<unsigned short>(data) != nullptr));

	wostringstream file_name;
	static unsigned int niumag_img_index = 0;
	file_name << ++niumag_img_index;

	auto output_folder = GetString(L"ExportFolder");
	wstring file_path = output_folder;
	if (wcslen(output_folder) > 3)
	{
		file_path += L"\\";
	}
	file_path += file_name.str();
	file_path += L".niuimg";

	//write data
	int file_version = 1;
	int section1size = 100;
	int section2size = 100;
	int section3size = 100;
	int section4size = 100;
	int section5size = 100;
	int section6size = 100; //data size
	int section7size = 100;
	int section8size = 100;

	int section6_offset = 4 * 9 
							+ section1size 
							+ section2size
							+ section3size
							+ section4size
							+ section5size;

	DataHelper data_helper(data);
	
	auto dimension_count = data_helper.GetDimensionCount();
	if (dimension_count != 2)
		return false;

	int dim1 = data_helper.GetWidth();
	int dim2 = data_helper.GetHeight();

	unsigned buffer_size = dim1 * dim2;
	unsigned short * img_data = GetDataArray<unsigned short>(data);

	ofstream file(file_path.c_str(), ios::binary);

	file.write(reinterpret_cast<char*>(&file_version), 4);
	file.write(reinterpret_cast<char*>(&section1size), 4);
	file.write(reinterpret_cast<char*>(&section2size), 4);
	file.write(reinterpret_cast<char*>(&section3size), 4);
	file.write(reinterpret_cast<char*>(&section4size), 4);
	file.write(reinterpret_cast<char*>(&section5size), 4);
	file.write(reinterpret_cast<char*>(&section6size), 4);
	file.write(reinterpret_cast<char*>(&section7size), 4);
	file.write(reinterpret_cast<char*>(&section8size), 4);

	file.seekp(section6_offset, ios::beg);
	file.write(reinterpret_cast<char*>(&dim1), 4);
	file.write(reinterpret_cast<char*>(&dim2), 4);
	file.write(reinterpret_cast<char*>(img_data), buffer_size * sizeof(unsigned short));
	file.close();

	return true;
}