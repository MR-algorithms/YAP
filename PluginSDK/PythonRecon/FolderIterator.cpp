#include "stdafx.h"
#include "FolderIterator.h"
#include "Implement\LogUserImpl.h"
#include "Client\DataHelper.h"
#include "Implement\DataObject.h"

#include <regex>
#include <string>
#include <vector>
#include <io.h>
#include <iomanip>
#include <ctime>
#include <direct.h>
#include <iostream>
#include <boost/filesystem.hpp>

#pragma warning(disable:4996)

using namespace Yap;
using namespace std;

FolderIterator::FolderIterator() :
	ProcessorImpl(L"FolderIterator")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeAll);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeAll);

	AddProperty<const wchar_t * const>(L"Path", L"", L"文件夹目录");
	AddProperty<bool>(L"RecursiveSearch", false, L"搜索当前Path目录下的子目录文件夹");
	AddProperty<const wchar_t * const>(L"RegularEx", L"", L"文件件搜索正则表达式");
}

FolderIterator::~FolderIterator()
{
}

FolderIterator::FolderIterator(const FolderIterator& rhs) : 
	ProcessorImpl(rhs)
{
}

bool FolderIterator::Input(const wchar_t * name, IData * data)
{
	assert(data == nullptr || (Inputs()->Find(name) != nullptr));

	auto path = GetProperty<const wchar_t * const>(L"Path");
	bool recursive = GetProperty<bool>(L"RecursiveSearch");
	/* regular expression */
	auto c_reg = GetProperty<const wchar_t* const>(L"RegularEx");
	//
	std::vector<std::wstring> folders;
	GetFolders(folders, path, recursive, c_reg);

	if (folders.empty())
	{
		LOG_ERROR(L"Property [Path] is not aproperate a directories", L"PythonRecon");
		return false;
	}

	for (auto iter : folders)
	{
		VariableSpace variables;
		variables.AddVariable(L"string", L"FolderPath", L"Full path of one sub-folder in the current folder");
		variables.Set(L"FolderPath", iter.c_str());
		variables.AddVariable(L"bool", L"Finished", L"Iteration finished.");
		variables.Set(L"Finished", false);

		auto output = DataObject<int>::CreateVariableObject(variables.Variables(), _module.get());

		Feed(L"Output", output.get());
	}
	
	NotifyIterationFinished();
	
	return true;
}

void Yap::FolderIterator::NotifyIterationFinished()
{
	VariableSpace variables;
	variables.AddVariable(L"bool", L"Finished", L"Iteration finished.");
	variables.Set(L"Finished", true);

	auto output = DataObject<int>::CreateVariableObject(variables.Variables(), _module.get());
	Feed(L"Output", output.get());
}

void FolderIterator::GetFolders(std::vector<std::wstring> & folders, std::wstring folders_path, 
	bool recursive,
	std::wstring regular_exp) 
{
	assert(!folders_path.empty());
	if (folders_path.length() >= _MAX_FNAME || int(folders_path.find(L' ')) != -1)
	{
		return ;
	}

 	if (folders_path.rfind(L'\\') == (folders_path.length() - 1))
 		folders_path.pop_back();
 
 	using namespace boost::filesystem;
	path p(folders_path);

	try
	{
		if (exists(p) && is_directory(p))
		{
			directory_iterator iter_end;
			for (directory_iterator iter(p); iter != iter_end; ++iter)
			{
				std::wstring dir_wname = (*iter).path().wstring();
				std::wstring file_wname = (*iter).path().filename().wstring();
				
				if (is_directory(*iter)) //
				{
					if (!regular_exp.empty())
					{
						std::wregex re(regular_exp);
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
				if (recursive)
				{
					GetFolders(folders, dir_wname.c_str(), recursive, regular_exp);
				}
			}
		}
		else
		{
			LOG_ERROR(L"there is not an valid path!", L"FolderIterator");
		}
	}
	catch (const filesystem_error& ex)
	{
		LOG_ERROR(L"boost filesystem read file error", L"FolderIterator");
	}
}
