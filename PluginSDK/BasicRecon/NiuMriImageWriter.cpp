#include "stdafx.h"
#include "NiuMriImageWriter.h"

#include <time.h>
#include <stdio.h>
#include <fstream>
#include <iosfwd>
#include "Interface\Client\DataHelper.h"

using namespace Yap;
using namespace std;

NiuMriImageWriter::NiuMriImageWriter(void) :
	ProcessorImpl(L"NiuMriImageWriter")
{
	AddInput(L"Input", 3, DataTypeUnsignedShort);
	_properties->Add(VariableString, L"ExportFolder", L"Set folder used to write images.");
	_properties->Add(VariableString, L"FileName", L"Set file name.");
}

NiuMriImageWriter::NiuMriImageWriter(const NiuMriImageWriter& rhs) :
	ProcessorImpl(rhs)
{
}

NiuMriImageWriter::~NiuMriImageWriter()
{
}

bool Yap::NiuMriImageWriter::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && (GetDataArray<unsigned short>(data) != nullptr));

	auto output_folder = _properties->Get<const wchar_t*>(L"ExportFolder");
	auto output_name = _properties->Get<const wchar_t*>(L"FileName");
	auto file_path = GetFilePath(output_folder, output_name);

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
	if (dimension_count != 3)
		return false;

	int dim1 = data_helper.GetWidth();
	int dim2 = data_helper.GetHeight();
	int dim3 = data_helper.GetSliceCount();

	unsigned buffer_size = dim1 * dim2 * dim3;
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
	file.write(reinterpret_cast<char*>(&dim3), 4);
	file.write(reinterpret_cast<char*>(img_data), buffer_size * sizeof(unsigned short));
	file.close();

	return true;
}

std::wstring Yap::NiuMriImageWriter::GetFilePath(const wchar_t * output_folder, const wchar_t * output_name)
{
	wstring file_path = output_folder;
	if (wcslen(output_folder) > 3)
	{
		file_path += L"\\";
	}

	if (wcslen(output_name) > 0)
	{
		file_path += output_name;
		file_path += L".";
	}

	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y%m%d", localtime(&t));
	string str(tmp);
	std::wstring wstr;
	wstr.assign(str.begin(), str.end());
	file_path += wstr;

	file_path += L".niuimg";

	return file_path;
}