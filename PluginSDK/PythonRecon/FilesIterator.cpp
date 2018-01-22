#include "stdafx.h"
#include "FilesIterator.h"
#include "Implement\LogUserImpl.h"
#include "Client\DataHelper.h"

#include <io.h>
#include <regex>

#pragma warning(disable:4996)

using namespace  Yap;
using namespace std;

string FilesIterator::ToMbs(const wchar_t * wcs)
{
	assert(wcslen(wcs) < 500 && "The string cannot contain more than 500 characters.");

	static char buffer[1024];
	size_t size;
	wcstombs_s(&size, buffer, (size_t)1024, wcs, (size_t)1024);
	return std::string(buffer);
}

FilesIterator::FilesIterator():
	ProcessorImpl(L"FilesIterator")
{
	AddInput(L"Input", 1, DataTypeChar);
	AddOutput(L"Output", 1, DataTypeChar);
	AddOutput(L"Reference", 1, DataTypeChar);
	
	AddProperty<const wchar_t* const>(L"FileDir", L"", L"文件的指定目录，该属性启用，则抑制从输入端口传入文件目录.");
	AddProperty<const wchar_t* const>(L"FileRegex", L"", L"文件名查找正则表达式");
	AddProperty<bool>(L"SearchSubDir", false, L"是否搜索子目录文件下的文件");
	AddProperty<const wchar_t* const>(L"ReferenceRegex", L"", L"选择参考图像的正则表达式(仅只能获得到一个参考图像).");

	/* THIS PROPERTY CAN NOT BE SETTED AT PIPELINE!!!*/
	AddProperty<bool>(L"FilesIteratorFinished", false,
		L"Mark FilesIterator has send patients' all modalities data or not. FORBIDEN SETTED IN PIPELINE!!!");

	LOG_TRACE(L"FilesIterator constructor", L"PythonRecon");
}

FilesIterator::FilesIterator(const FilesIterator & rhs) : 
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"FilesIterator copy constructor", L"PythonRecon");
}

FilesIterator::~FilesIterator()
{
	LOG_TRACE(L"FilesIterator destructor", L"PythonRecon");
}

bool Yap::FilesIterator::Input(const wchar_t * name, IData * data)
{
	const wchar_t * file_dir = GetProperty<const wchar_t* const>(L"FileDir");
	const wchar_t * file_regex = GetProperty<const wchar_t * const>(L"FileRegex");
	const wchar_t * reference_regex = GetProperty<const wchar_t * const>(L"ReferenceRegex");
	bool is_subdir = GetProperty<bool>(L"SearchSubDir");
	
	std::string file_path;
	if (wcslen(file_dir) == 0)
	{
		if (data == nullptr)
			return false;

		if (data->GetDataType() != DataTypeChar)
			return false;
		file_path = GetDataArray<char>(data);
	}
	else
	{
		file_path = ToMbs(file_dir);
	}
	
	std::vector<string> file_container;
	GetFiles(file_path, file_container, is_subdir);

	if (file_container.size() == 0)
		return false;

	DataHelper helper(data);
	auto dim = helper.GetDimension(DimensionUser1);
	
	SetProperty<bool>(L"FilesIteratorFinished", false);
	if (wcslen(reference_regex)==0)
	{
		unsigned int count = 0;
		for (auto iter:file_container)
		{
			Dimensions dimension;
			if (dim.type != DimensionInvalid)
			{
				dimension(dim.type, dim.start_index, dim.length)
					(DimensionUser2, count, unsigned(file_container.size()))
					(DimensionUser3, 0U, unsigned(iter.length() + 1));
			}
			else
			{
				dimension(DimensionUser2, count, unsigned(file_container.size()))
					(DimensionUser3, 0U, unsigned(iter.length() + 1));
			}
			++count;
			char * tempt = new char[iter.length()+1];
			strcpy(tempt, iter.c_str());
			auto out_data = CreateData<char>(nullptr, tempt, dimension);
			Feed(L"Output", out_data.get());
		}
		SetProperty<bool>(L"FilesIteratorFinished", true);
	}
	else
	{
		unsigned int count = 0;
		auto reg = regex(ToMbs(reference_regex).data());
		for (auto iter : file_container)
		{
			if (regex_match(iter,reg))
			{
				Dimensions dimension;
				if(dim.type != DimensionInvalid)
					dimension(dim.type,dim.start_index, dim.length)
						(DimensionUser2, count, unsigned(file_container.size() - 1))
						(DimensionUser3, 0U, unsigned(iter.length()+1));
				else
					dimension(DimensionUser2, count, unsigned(file_container.size() - 1))
						(DimensionUser3, 0U, unsigned(iter.length() + 1));
				++count;
				char * my_char = new char[iter.length()+1];
				strcpy(my_char, iter.c_str());
				auto out_data = CreateData<char>(nullptr, my_char, dimension);
				Feed(L"Reference", out_data.get());
				break;
			}
		}

		for (auto iter : file_container)
		{
			if (!regex_match(iter, regex(ToMbs(reference_regex))))
			{
				Dimensions dimension;
				if (dim.type != DimensionInvalid)
					dimension(dim.type, dim.start_index, dim.length)
					(DimensionUser2, count, unsigned(file_container.size() - 1))
					(DimensionUser3, 0U, unsigned(iter.length() + 1));
				else
					dimension(DimensionUser2, count, unsigned(file_container.size() - 1))
					(DimensionUser3, 0U, unsigned(iter.length() + 1));
				++count;
				char * my_char = new char[iter.length() + 1];
				strcpy(my_char, iter.c_str());
				auto out_data = CreateData<char>(nullptr, my_char, dimension);
				Feed(L"Output", out_data.get());
			}
		}
		SetProperty<bool>(L"FilesIteratorFinished", true);
	}
	return true;
}

void Yap::FilesIterator::GetFiles(std::string file_path, std::vector<std::string>& folders, bool is_subfolder)
{
	assert(file_path.length() != 0);
	if (file_path.length() >= _MAX_FNAME || int(file_path.find(' ')) != -1)
		return;

	if (file_path.rfind('\\') == (file_path.length() - 1))
		file_path.pop_back();

	long long   hFile = 0;
	_finddata_t file_info;// file information struct.
	string temp;

	if ((hFile = _findfirst(((temp.assign(file_path)).append("/*")).c_str(), &file_info)) != -1)
	{
		do
		{
			if ((file_info.attrib & _A_SUBDIR)) 
			{
				if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0)
				{
					if (is_subfolder)
					{
						GetFiles(temp.assign(file_path).append("//").append(file_info.name), folders, is_subfolder);
					}
				}
			}
			else
			{
				folders.push_back(((temp.assign(file_path)).append("//")).append(file_info.name));
			}
		} while (_findnext(hFile, &file_info) == 0);
		_findclose(hFile);
	}
}
