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

using namespace Yap;
using namespace std;

FolderIterator::FolderIterator() :
	ProcessorImpl(L"FolderIterator")
{
	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeAll);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeAll);

	AddProperty<const wchar_t * const>(L"Path", L"", L"文件夹目录");
	AddProperty<bool>(L"SearchSubDir", false, L"搜索当前Path目录下的子目录文件夹");
	AddProperty<const wchar_t * const>(L"Regular", L"", L"文件件搜索正则表达式");

	LOG_TRACE(L"FolderIterator constructor called.", L"BasicRecon");
}

FolderIterator::~FolderIterator()
{
	LOG_TRACE(L"FolderIterator destructor", L"BasicRecon");
}

FolderIterator::FolderIterator(const FolderIterator& rhs) : 
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"FolderIterator copy constructor", L"BasicRecon");
}

bool FolderIterator::Input(const wchar_t * name, IData * data)
{
	assert(data == nullptr || (Inputs()->Find(name) != nullptr));

	string path = ToMbs(GetProperty<const wchar_t * const>(L"Path"));
	bool is_subfolder = GetProperty<bool>(L"SearchSubDir");

	vector<string> folders;
	GetFolders(path, folders, is_subfolder);

	if (folders.empty())
	{
		LOG_ERROR(L"Property [Path] is not aproperate a directories", L"BasicRecon");
		return false;
	}

	/* regular */
	const wchar_t * c_reg = GetProperty<const wchar_t* const>(L"Regular");
	if (wcslen(c_reg) == 0)
	{
		for (auto iter:folders)
		{
			Dimensions dimension;
			dimension(DimensionReadout, 0U, unsigned(iter.length()));
			char * tempt = new char[iter.length()];
			strcpy(tempt, iter.c_str());
			auto out_data = CreateData<char>(nullptr, tempt, dimension, nullptr, true);
			Feed(L"Output", out_data.get());
		}
	}
	else
	{
		std::regex re(ToMbs(c_reg));
		for (auto iter : folders)
		{
			if (regex_match(iter, re))
			{
				Dimensions dimension;
				dimension(DimensionReadout, 0U, unsigned(iter.length()));
				char * tempt = new char[iter.length()];
				strcpy(tempt, iter.c_str());
				auto out_data = CreateData<char>(nullptr, tempt, dimension, nullptr, true);
				Feed(L"Output", out_data.get());
			}
		}
	}
	return true;
}

string FolderIterator::ToMbs(const wchar_t * wcs)
{
	assert(wcslen(wcs) < 500 && "The string cannot contain more than 500 characters.");

	static char buffer[1024];
	size_t size;
	wcstombs_s(&size, buffer, (size_t)1024, wcs, (size_t)1024);
	return std::string(buffer);
}

void FolderIterator::GetFolders(string path, vector<string>& folders,bool is_subfolder) 
{
	assert(path.length() != 0);
	if (path.length() >= _MAX_FNAME || int(path.find(' ')) != -1)
		return;

	if (path.rfind('\\') == (path.length() - 1))
		path.pop_back();

	long long   hFile = 0;
	_finddata_t file_info;// file information struct.
	string temp;

	if ((hFile = _findfirst(((temp.assign(path)).append("/*")).c_str(), &file_info)) != -1)
	{
		do 
		{
			if ((file_info.attrib & _A_SUBDIR)) 
			{
				if (strcmp(file_info.name, ".") != 0 && strcmp(file_info.name, "..") != 0)
				{	
					folders.push_back(((temp.assign(path)).append("//")).append(file_info.name));
					if (is_subfolder)
					{
						GetFolders(temp.assign(path).append("//").append(file_info.name), folders, is_subfolder);
					}
				}
			}
		} while (_findnext(hFile, &file_info) == 0);

		_findclose(hFile);
	}
}
