#include "stdafx.h"
#include "FileIterator.h"
#include "Implement\LogUserImpl.h"
#include "Client\DataHelper.h"
#include "Implement\VariableSpace.h"

#include <io.h>
#include <regex>
#include <boost/filesystem.hpp>

#pragma warning(disable:4996)

using namespace  Yap;
using namespace std;

FileIterator::FileIterator():
	ProcessorImpl(L"FileIterator")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeAll);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeAll);
	AddOutput(L"Reference", YAP_ANY_DIMENSION, DataTypeAll);
	
	AddProperty<std::wstring>(L"FolderPath", L"", L"文件的指定目录,从输入端口传入文件目录可以抑制该属性的启用.");
	AddProperty<std::wstring>(L"FileRegEx", L"", L"文件名查找正则表达式");
	AddProperty<bool>(L"RecursiveSearch", false, L"是否搜索子目录文件下的文件");
	AddProperty<std::wstring>(L"ReferenceRegEx", L"", L"选择参考图像的正则表达式(仅只能获得到一个参考图像).");
}

FileIterator::FileIterator(const FileIterator & rhs) : 
	ProcessorImpl(rhs)
{
}

FileIterator::~FileIterator()
{
}

bool Yap::FileIterator::Input(const wchar_t * name, IData * data)
{
	wstring folder_path;
	wstring file_regex;
	wstring reference_regex;
	bool is_recursive = false;

	assert(data != nullptr);

	if (data->GetVariables() != nullptr)
	{
		VariableSpace variables(data->GetVariables());
		if (variables.Get<bool>(L"FolderFinished"))
		{
			NotifyIterationFinished(data);
			return true;
		}
		try{
			folder_path = variables.Get<std::wstring>(L"FolderPath");
		}catch (VariableException &){}

		//file_regex = variables.Get<const wchar_t * const>(L"FileRegEx");
		//reference_regex = variables.Get<const wchar_t * const>(L"ReferenceRegEx");
		//is_recursive = variables.Get<bool>(L"RecursiveSearch");
	}

	if (folder_path.empty())
	{
		folder_path = GetProperty<std::wstring>(L"FolderPath");
	}
	if (file_regex.empty())
	{
		file_regex = GetProperty<std::wstring>(L"FileRegEx");
	}
	if (reference_regex.empty())
	{
		reference_regex = GetProperty<std::wstring>(L"ReferenceRegEx");
	}
	is_recursive = GetProperty<bool>(L"RecursiveSearch");

	std::vector<std::wstring> files_vector;
 	GetFiles(files_vector, folder_path, is_recursive, file_regex);
 
 	if (files_vector.empty())
 	{
 		return false;
 	}
 
 	if (!reference_regex.empty())
 	{
 		auto reg = wregex(reference_regex);
 		for (auto iter = files_vector.begin(); iter != files_vector.end(); ++iter)
 		{
 			if (regex_match(*iter, reg))
 			{
 				VariableSpace variables(data->GetVariables());
 				variables.AddVariable(L"string", L"FilePath", L"Full path of one sub-folder in the current folder");
 				variables.Set<std::wstring>(L"FilePath", iter->c_str());

				variables.AddVariable(L"bool", L"FileFinished", L"Iteration finished.");
				variables.Set<bool>(L"FileFinished", false);

 				auto output = DataObject<float>::CreateVariableObject(variables.Variables(), _module.get());
 				Feed(L"Reference", output.get());
 				files_vector.erase(iter);
 				break;
 			}
 		}
 	}
 
 	for (auto& file_path : files_vector)
 	{
 		VariableSpace variables(data->GetVariables());
 		variables.AddVariable(L"string", L"FilePath", L"Full path of one sub-folder in the current folder");
 		variables.Set<std::wstring>(L"FilePath", file_path.c_str());

 		variables.AddVariable(L"bool", L"FileFinished", L"Iteration finished.");
 		variables.Set<bool>(L"FileFinished", false);
 
 		auto output = DataObject<float>::CreateVariableObject(variables.Variables(), _module.get());

 		Feed(L"Output", output.get());
 	}

	NotifyIterationFinished(data);

	return true;
}

void Yap::FileIterator::NotifyIterationFinished(IData * data)
{
	VariableSpace variables(data->GetVariables());
	variables.AddVariable(L"bool", L"FileFinished", L"Iteration finished.");
	variables.Set(L"FileFinished", true);

	auto output = DataObject<float>::CreateVariableObject(variables.Variables(), _module.get());
	Feed(L"Output", output.get());
}

void Yap::FileIterator::GetFiles(std::vector<std::wstring>& folders, 
	std::wstring file_path, 
	bool is_recursive, 
	std::wstring regular_exp)
{
	assert(!file_path.empty());
	if (file_path.length() >= _MAX_FNAME || int(file_path.find(' ')) != -1)
		return;

	if (file_path.rfind('\\') == (file_path.length() - 1))
		file_path.pop_back();

	using namespace boost::filesystem;

	path p(file_path);

	try
	{
		if (exists(p) && is_directory(p))
		{
			directory_iterator iter_end;
			for (directory_iterator iter(p); iter != iter_end; ++iter)
			{
				std::wstring dir_wname = (*iter).path().wstring();
				std::wstring file_wname = (*iter).path().filename().wstring();

				if (is_regular_file(*iter))
				{
					if (!regular_exp.empty())
					{
						wregex re(regular_exp);
						if (regex_match(file_wname, re))
						{
							folders.push_back(dir_wname);
						}
					}
					else
					{
						folders.push_back(dir_wname);
					}
				}
				else if(is_directory(*iter)) //
				{
					if (is_recursive)
					{
						GetFiles(folders, dir_wname, is_recursive, regular_exp);
					}
				}
			}
		}
		else
		{
			LOG_ERROR(L"there is not an valid path!", L"FolderIterator");
		}
	}
	catch (const filesystem_error& )
	{
		LOG_ERROR(L"boost filesystem read file error", L"FolderIterator");
	}

}
