#include "stdafx.h"
#include "NiumagFidWriter.h"

#include <fstream>
#include <iosfwd>
#include "Client\DataHelper.h"

using namespace Yap;
using namespace std;

NiumagFidWriter::NiumagFidWriter(void) :
	ProcessorImpl(L"NiumagFidWriter")
{
	AddInput(L"Input", 4, DataTypeComplexFloat);
	_properties->Add(VariableString, L"ExportFolder", L"Set folder used to write fid.");
}

NiumagFidWriter::NiumagFidWriter(const NiumagFidWriter& rhs) :
	ProcessorImpl(rhs)
{
}

NiumagFidWriter::~NiumagFidWriter()
{
}

bool Yap::NiumagFidWriter::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && (GetDataArray<complex<float>>(data) != nullptr));

	wostringstream file_name;
	static unsigned int niumag_img_index = 0;
	file_name << ++niumag_img_index;

	auto output_folder = _properties->Get<const wchar_t*>(L"ExportFolder");
	wstring file_path = output_folder;
	if (wcslen(output_folder) > 3)
	{
		file_path += L"\\";
	}
	file_path += file_name.str();
	file_path += L".img.fid";

	//write data
	int file_version = 1;
	int section1size = 100;
	int section2size = 100;
	int section3size = 100;
	int section4size = 100;
	int section5size = 100; //fid
	
	int section5_offset = 4 * 6 + 400;

	DataHelper data_helper(data);

	auto dimension_count = data_helper.GetDimensionCount();
	if (dimension_count != 4)
		return false;

	int dim1 = data_helper.GetWidth();
	int dim2 = data_helper.GetHeight();
	int dim3 = data_helper.GetSliceCount();
	int dim4 = data_helper.GetDim4();

	unsigned buffer_size = dim1 * dim2 * dim3 * dim4;
	complex<float> * fid_data = GetDataArray<complex<float>>(data);

	ofstream file(file_path.c_str(), ios::binary);

	file.write(reinterpret_cast<char*>(&file_version), 4);
	file.write(reinterpret_cast<char*>(&section1size), 4);
	file.write(reinterpret_cast<char*>(&section2size), 4);
	file.write(reinterpret_cast<char*>(&section3size), 4);
	file.write(reinterpret_cast<char*>(&section4size), 4);
	file.write(reinterpret_cast<char*>(&section5size), 4);

	file.seekp(section5_offset, ios::beg);
	file.write(reinterpret_cast<char*>(&dim1), 4);
	file.write(reinterpret_cast<char*>(&dim2), 4);
	file.write(reinterpret_cast<char*>(&dim3), 4);
	file.write(reinterpret_cast<char*>(&dim4), 4);
	file.write(reinterpret_cast<char*>(fid_data), buffer_size * sizeof(complex<float>));
	file.close();

	return true;
}