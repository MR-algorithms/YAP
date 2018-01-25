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
}

FilesIterator::FilesIterator(const FilesIterator & rhs) : 
	ProcessorImpl(rhs)
{
}

FilesIterator::~FilesIterator()
{
}

bool Yap::FilesIterator::Input(const wchar_t * name, IData * data)
{
	wstring folder_path;
	wstring file_regex;
	wstring reference_regex;

	assert(data != nullptr);

	if (data->GetVariables() != nullptr)
	{
		VariableSpace variables(data->GetVariables());
		folder_path = variables.Get<const wchar_t *>(L"FolderPath");
		file_regex = variables.Get<const wchar_t *>(L"FileRegEx");
		reference_regex = variables.Get<const wchar_t *>(L"ReferenceRegEx");
	}

	if (folder_path.empty())
	{
		folder_path = GetProperty<const wchar_t* const>(L"FileDir");
	}
	if (file_regex.empty())
	{
		file_regex = GetProperty<const wchar_t * const>(L"FileRegex");
	}
	if (reference_regex.empty())
	{
		reference_regex = GetProperty<const wchar_t * const>(L"ReferenceRegex");
	}

	bool is_subdir = GetProperty<bool>(L"SearchSubDir");

	std::wstring file_path;

	std::vector<string> file_container;
// 	GetFiles(file_path, file_container, is_subdir);
// 
// 	if (file_container.empty())
// 	{
// 		return false;
// 	}
// 
// 	if (!reference_regex.empty())
// 	{
// 		auto reg = regex(ToMbs(reference_regex.c_str()).data());
// 		for (auto iter = file_container.begin(); iter != file_container.end(); ++iter)
// 		{
// 			if (regex_match(*iter, reg))
// 			{
// 				VariableSpace variables;
// 				variables.Add(L"string", L"FilePath", L"Full path of one sub-folder in the current folder");
// 				// YG: variables.Set(L"FilePath", iter.c_str());
// 
// 				auto output = DataObject<int>::CreateVariableObject(variables.Variables(), _module.get());
// 				Feed(L"Reference", output.get());
// 				file_container.erase(iter);
// 				break;
// 			}
// 		}
// 	}
// 
// 	for (auto& file_path : file_container)
// 	{
// 		VariableSpace variables;
// 		variables.Add(L"string", L"FilePath", L"Full path of one sub-folder in the current folder");
// 		variables.Set(L"FilePath", file_path.c_str());
// 		variables.Add(L"bool", L"Finished", L"Iteration finished.");
// 		variables.Set(L"Finished", false);
// 
// 		auto output = DataObject<int>::CreateVariableObject(variables.Variables(), _module.get());
// 		Feed(L"Output", output.get());
// 	}

	NotifyIterationFinished();

	return true;
}

void Yap::FilesIterator::NotifyIterationFinished()
{
	VariableSpace variables;
	variables.Add(L"bool", L"Finished", L"Iteration finished.");
	variables.Set(L"Finished", true);

	auto output = DataObject<int>::CreateVariableObject(variables.Variables(), _module.get());
	Feed(L"Output", output.get());
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
