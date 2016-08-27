// Yang Guang, reviewed 2007-03-20

#include "stdafx.h"
#include "FileSystem.h"

#include <string>
#include <fstream>
#include <cassert>

#include <comutil.h>

#ifndef NO_BOOST
#include <boost/tokenizer.hpp>
#include <boost/mem_fn.hpp>
#endif

#include <list>
#include "scrrun.h"
#include <afx.h>

#define SAFE_RELEASE(o) if(o != NULL){o->Release(); o = NULL;}

using namespace std;

namespace FileSystem
{

	bool DeleteMultiFile(const CString& directory, const CString& filename)
	{
		bool return_code = true;

		CFileFind finder;
		BOOL found = finder.FindFile(directory + _T("\\") + filename);
		while (found)
		{
			found = finder.FindNextFile();
			if (!finder.IsDots())
			{
				::SetFileAttributes(finder.GetFilePath(), FILE_ATTRIBUTE_NORMAL);
				if (!(::DeleteFile(finder.GetFilePath())))
				{
					return_code = false;
				}
			}
		}
		return return_code;
	}

	bool IsFileReadonly(const CString& filepath)
	{
		CFileStatus status;
		if (!CFile::GetStatus(filepath, status))
		{
			throw "File not found";
		}
		return (status.m_attribute & CFile::readOnly);
	}

	bool DeleteDirectory(const CString& directory, bool bDeleteItself)
	{
		bool return_code = true;

		CFileFind finder;

		CString path = directory + ((directory[directory.GetLength() - 1] == '\\') ? "*.*" : "\\*.*");
		BOOL found = finder.FindFile(path);
		while (found)
		{
			found = finder.FindNextFile();
			if (!finder.IsDots())
			{
				if (finder.IsDirectory())
				{
					if (!DeleteDirectory(finder.GetFilePath().GetString()))
					{
						return_code = false;
					}
				}
				else
				{
					::SetFileAttributes(finder.GetFilePath(), FILE_ATTRIBUTE_NORMAL);
					if (!::DeleteFile(finder.GetFilePath()))
					{
						return_code = false;
					}
				}
			}
		}

		finder.Close();

		if (bDeleteItself)
		{
			CString dir = directory;
			if (dir[dir.GetLength() - 1] == _T('\\'))
			{
				dir = dir.Left(dir.GetLength() - 1);
			}
			::SetFileAttributes(dir, FILE_ATTRIBUTE_NORMAL);
			if (!::RemoveDirectory(dir))
			{
				return false;
			}
		}

		return return_code;
	}

	CString FindFileInDirectory(const CString& directory_path,
		const CString& file_name,
		bool include_subdirectory)
	{
		CString path = directory_path + _T("\\");
		path += (file_name.IsEmpty() ? TEXT("*.*") : file_name);

		CFileFind finder;
		bool working = (finder.FindFile(path) == TRUE);
		while (working)
		{
			working = (finder.FindNextFile() == TRUE);
			if (finder.IsDirectory())
			{
				continue;
			}

			return finder.GetFilePath();
		}

		if (include_subdirectory)
		{
			CFileFind folder_finder;
			bool folder_finder_working = (folder_finder.FindFile(directory_path + _T("\\*.*")) == TRUE);
			while (folder_finder_working)
			{
				folder_finder_working = (folder_finder.FindNextFile() == TRUE);
				if (folder_finder.IsDots())
				{
					continue;
				}
				else if (folder_finder.IsDirectory())
				{
					CString result = FindFileInDirectory(folder_finder.GetFilePath(), file_name, true);
					if (!result.IsEmpty())
					{
						return result;
					}
				}
			}
		}

		return CString();
	}

	// 为了便于目录比较，简化非法路径的定义：除名字中间外，不能有空格．
	// 为支持尾部空格，可以在获取路径时，去掉尾部空格。
	// 同样简化了判断规则，主要检查空格，没有检查其他字符

	bool IsValidPath(const CString& path)
	{
		// 两边都不允许有空格
		CString trimmed = path;
		trimmed.Trim();

		if (trimmed.GetLength() != path.GetLength())
			return false;

		if (path.GetLength() < 2)
		{
			return false;
		}
		if (path.GetLength() == 2)
		{
			return (((path[0] >= _T('A') && path[0] <= _T('Z')) || (path[0] >= _T('a') && path[0] <= _T('z'))) && path[1] == _T(':'));
		}
		if (!(((path[0] >= _T('A') && path[0] <= _T('Z')) || (path[0] >= _T('a') && path[0] <= _T('z'))) && path[1] == _T(':')))
			return false;

		// 检查'/'两边是否有空格
		if (path.Find(_T(" \\")) != -1 || path.Find(_T("\\ ")) != -1 || path.Find(_T("\t\\")) != -1 || path.Find(_T("\\\t")) != -1)
			return false;

		return true;
	}

	// 比较两个目录是否相等。注意：参数必须为目录
	bool IsFolderEqual(const CString& dir1, const CString& dir2)
	{
		if (!IsValidPath(dir1) || !IsValidPath(dir2))
		{
			return false;
		}
		CString upper_dir1 = dir1;
		CString upper_dir2 = dir2;
		return (upper_dir1.MakeUpper() == upper_dir2.MakeUpper());
	}

	//试图把所有的驱动器当作文件或目录看待
	bool FileExists(const CString& filename)
	{
		CString str = filename;
	
		str.TrimRight(_T('\\'));	// for root directory.

		if (str[str.GetLength() - 1] == _T(':'))
		{
			switch (GetDriveType(str))
			{
			case DRIVE_UNKNOWN:
			case DRIVE_NO_ROOT_DIR:
				return false;
			case DRIVE_REMOVABLE:
			case DRIVE_FIXED:
			case DRIVE_REMOTE:
			case DRIVE_CDROM:
			case DRIVE_RAMDISK:
				return true;
			default:
				return false;
			}
		}
		else if (str.GetLength() <= 3)
		{
			return false;
		}
		else
		{
			CFileStatus status;
			return (CFile::GetStatus(filename, status) != 0);
		}
	}

	bool IsDirectoryEmpty(const CString& directory)
	{
		// 判断目录是否存在
		if (!FileExists(directory))
			return true;

		CFileFind finder;
		CString path = directory + ((directory[directory.GetLength() - 1] == _T('\\')) ? _T("*.*") : _T("\\*.*"));
		BOOL found = finder.FindFile(path);
		while (found)
		{
			found = finder.FindNextFile();
			if (!finder.IsDots())
			{
				return false;
			}
		}
		return true;
	}

	bool CopyDirectory(const CString& source,
		const CString& destination,
		bool ihe_compatible)
	{
		CString dest_folder = destination;
		if (ihe_compatible)
		{
			dest_folder.MakeUpper();
		}

		if (!CreateFolderTree(dest_folder))
			return false;

		CFileFind finder;
		CString files = source + (source[source.GetLength() - 1] == _T('\\') ? _T("*.*") : _T("\\*.*"));
		BOOL found = finder.FindFile(files);
		while (found)
		{
			found = finder.FindNextFile();
			if (!finder.IsDots())
			{
				CString destination_path = dest_folder;
				if (destination[dest_folder.GetLength() - 1] != _T('\\'))
				{
					destination_path += _T('\\');
				}

				CString dest_file = finder.GetFileName();
				if (ihe_compatible)
				{
					dest_file.MakeUpper();
					dest_file.Replace(_T(".DCM"), _T("DCM"));
				}
				destination_path += dest_file;

				if (finder.IsDirectory())
				{
					CopyDirectory(finder.GetFilePath(), destination_path, ihe_compatible);
				}
				else
				{
					::CopyFile(finder.GetFilePath(), destination_path, false);
				}
			}
		}
		finder.Close();

		return true;
	}

	bool CreateFolder(const CString& folder)
	{
		if (::PathFileExists(folder))
			return true;

		// 如果以\结尾，将不能找到。
		CString folder_path = folder;
		folder_path.TrimRight(_T("\\"));

		CFileFind finder;
		if (finder.FindFile(folder_path))
		{
			finder.FindNextFile();

			return (finder.IsDirectory() != FALSE); // found, already exist
		}

		return (::CreateDirectory(folder_path, NULL) != 0);
	}

	bool CreateFolderTree(const CString& path)
	{
		if (FileExists(path))
			return true;

		CString parent = path.Left(path.ReverseFind(_T('\\')));
		return (CreateFolderTree(parent) && CreateFolder(path));
	};

	bool GetFolderSize3(const CString& folder, ULONGLONG &dwLength)
	{
		CFileFind finder;
		CString folder_files = folder + ((folder[folder.GetLength() - 1] == _T('\\')) ? _T("*.*") : _T("\\*.*"));
		BOOL found = finder.FindFile(folder_files);

		while (found)
		{
			found = finder.FindNextFile();
			if (!finder.IsDots())
			{
				if (finder.IsDirectory())
				{
					GetFolderSize3(finder.GetFilePath().GetString(), dwLength);
				}
				else
				{
					CFile file(finder.GetFilePath(), CFile::modeRead | CFile::shareDenyNone);
					dwLength += file.GetLength();
				}
			}
		}
		finder.Close();

		return true;
	}

	long GetFolderSize2(const CString& folder)
	{
		ULONGLONG dwLength = 0;
		GetFolderSize3(folder, dwLength);
		return static_cast<long>(dwLength / 1024);
	}

	long GetFolderSize(const CString& folder_path)
	{
		//	CoInitialize(NULL);

		BSTR folder_path_bstr = _bstr_t(folder_path.GetString()).copy();

		IFileSystem *file_system = NULL;
		IFolder *folder = NULL;
		LONG return_value = -1;
		HRESULT hr = CoCreateInstance(CLSID_FileSystemObject, NULL, CLSCTX_INPROC_SERVER,
			IID_IFileSystem, (void**)&file_system);
		if (FAILED(hr))
		{
			return_value = GetFolderSize2(folder_path);
			goto quit;
		}

		hr = file_system->GetFolder(folder_path_bstr, &folder);
		if (FAILED(hr))
		{
			return_value = GetFolderSize2(folder_path);
			goto quit;
		}

		if (folder)
		{
			VARIANT vt = { 0 };
			folder->get_Size(&vt);
			return_value = vt.lVal / 1024;
		}
	quit:

		SAFE_RELEASE(folder);
		SAFE_RELEASE(file_system);

		//	CoUninitialize();

		return return_value;
	}


	// 输入和返回均不带尾缀'\'.
	CString GetFolderFromPath(const CString& path)
	{
		int pos = path.ReverseFind('\\');
		if (pos != -1)
		{
			return path.Left(pos);
		}
		return _T("");
	}

	CString GetFileNameFromPath(const CString& path)
	{
		int start_pos = path.ReverseFind('\\') + 1;
		int end_pos = path.ReverseFind('.');
		if (end_pos == -1)
		{
			end_pos = path.GetLength();
		}

		return path.Mid(start_pos, end_pos - start_pos);
	}

	CString GetFileExtensionFromPath(const CString& path)
	{
		int pos = path.ReverseFind('.');
		if (pos == -1)
		{
			return CString();
		}
		else
		{
			return path.Right(path.GetLength() - pos - 1);
		}
	}

	// 返回Mbytes
	typedef BOOL(WINAPI *PGETDISKFREESPACEEX)(LPCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);

	int GetDiskFreeSpace(LPCSTR pszDrive)
	{
		PGETDISKFREESPACEEX pGetDiskFreeSpaceEx;
		__int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;
		DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;
		BOOL bResult;
		int mBytes = -1;

		pGetDiskFreeSpaceEx = (PGETDISKFREESPACEEX)GetProcAddress(
			GetModuleHandle(_T("kernel32.dll")), "GetDiskFreeSpaceExA");

		if (pGetDiskFreeSpaceEx)
		{
			bResult = pGetDiskFreeSpaceEx(pszDrive,
				(PULARGE_INTEGER)&i64FreeBytesToCaller,
				(PULARGE_INTEGER)&i64TotalBytes,
				(PULARGE_INTEGER)&i64FreeBytes);

			// Process GetDiskFreeSpaceEx results.
			if (bResult)
			{
				TRACE("Total free bytes = %I64d\n", i64FreeBytes);
				mBytes = static_cast<int>(i64FreeBytes / (1024 * 1024));
			}
		}

		else
		{
			bResult = GetDiskFreeSpaceA(pszDrive,
				&dwSectPerClust,
				&dwBytesPerSect,
				&dwFreeClusters,
				&dwTotalClusters);

			// Process GetDiskFreeSpace results.
			if (bResult)
			{
				TRACE("Total free bytes = I64d\n",
					dwFreeClusters*dwSectPerClust*dwBytesPerSect);
				mBytes = static_cast<int>(dwFreeClusters*dwSectPerClust*dwBytesPerSect / (1024 * 1024));
			}
		}
		return mBytes;
	}

	/**
	Parse one line of the config file.
	\remarks The format of one line of the config file should be:
	<variable> = <value>.
	@param strSource The string of the line input.
	@param strVariable Output parameter used to hold the token representing
	the variable's name.
	@param strValue Output parameter used to hold the token representing the
	variable's value.
	@return @a true if successful, @a false otherwise.
	*/
	bool ParseLine(const TCHAR* line,
		CString& variable,
		CString& value)
	{
		variable.Empty();
		value.Empty();

		// 支持行说明
		CString line_to_parse = line;
		line_to_parse.Trim();
		int comment_index = line_to_parse.Find(_T("//"));
		if (comment_index != -1)
		{
			line_to_parse = line_to_parse.Left(comment_index);
		}
		if (line_to_parse.IsEmpty())
		{
			return false;
		}

		// Extract the left-hand part.
		// remove leading spaces.
		int equal_index = line_to_parse.Find(_T("="));
		if (equal_index != -1)
		{
			variable = line_to_parse.Left(equal_index);
			variable.Trim();
			value = line_to_parse.Mid(equal_index + 1);
			variable.Trim();

			return true;
		}

		return false;
	}

	/**
	Parse one line of the config file.
	\remarks The format of one line of the config file should be:
	<variable> = <value>.
	@param strSource The string of the line input.
	@param strVariable Output parameter used to hold the token representing
	the variable's name.
	@param strValue Output parameter used to hold the token representing the
	variable's value.
	@return @a true if successful, @a false otherwise.
	*/
	bool ParseLine(const std::string& strSource,
		std::string& strVariable,
		std::string& strValue)
	{
		//支持行说明
		if (strSource.substr(0, 2) == "//")
			return false;

		// Extract the left-hand part.
		// remove leading spaces.
		auto nPosStart = strSource.find_first_not_of(" \t");
		if (nPosStart == strSource.npos)
			return false;

		auto nPosEnd = strSource.find_first_of(" \t=", nPosStart);
		if (nPosStart == strSource.npos)
			return false;
		strVariable = strSource.substr(nPosStart, nPosEnd - nPosStart);

		// Extract the right-hand part.
		nPosStart = strSource.find('=');
		if (nPosStart == strSource.npos)
			return false;

		nPosStart = strSource.find_first_not_of(" \t", nPosStart + 1);
		if (nPosStart == strSource.npos)
			return false;
		nPosEnd = strSource.find_last_not_of(" \t");
		strValue = strSource.substr(nPosStart, nPosEnd - nPosStart + 1);

		return true;
	}

	CString GetTempFilePath(const CString& prefix)
	{
		const int BUFSIZE = 4096;
		DWORD  dwBufSize = BUFSIZE;
		static TCHAR szTempName[MAX_PATH];
		static TCHAR buffer[BUFSIZE];
		static TCHAR lpPathBuffer[BUFSIZE];

		// Get the temp path

		GetTempPath(dwBufSize,   // length of the buffer
			lpPathBuffer);      // buffer for path

		GetTempFileName(lpPathBuffer, // directory for temp files
			prefix.Left(3),                    // temp file name prefix
			0,                        // create unique name
			szTempName);              // buffer for name

		return szTempName;
	}

	CString GetStartPath()
	{
		TCHAR buffer[256];
		CString path;
		if (GetModuleFileName(GetModuleHandle(NULL), buffer, 256) != 0)
		{
			path = GetFolderFromPath(buffer);
		}
		return path;
	}

#ifndef NO_BOOST
	CString SimplifyPath(const CString& path)
	{
		std::string path_string = CT2CA(path);
		char_separator<char> separator("\\");
		tokenizer<char_separator<char> > tokens(path_string, separator);
		tokenizer<char_separator<char> >::iterator iter = tokens.begin();
		std::list<CString> path_nodes;
		for (; iter != tokens.end(); ++iter)
		{
			CString temp = CString(CA2T((*iter).c_str()));
			if (temp != _T(".."))
			{
				path_nodes.push_back(temp);
			}
			else
			{
				path_nodes.pop_back();
			}
		}
		CString simp_path;
		for (auto it = path_nodes.begin(); it != path_nodes.end(); ++it)
		{
			simp_path += *it + _T("\\");
		}
		if (path.Right(1) != _T("\\"))
		{
			simp_path = simp_path.Left(simp_path.GetLength() - 1);
		}
		return simp_path;
	}
#endif

	CString GetCurrentDirectory()
	{
		DWORD buffer_size = ::GetCurrentDirectory(0, NULL);
		try
		{
			TCHAR* buffer = new TCHAR[buffer_size];
			::GetCurrentDirectory(buffer_size, buffer);

			return buffer;
		}
		catch (std::bad_alloc&)
		{
			return CString();
		}
	}

	CFileHelper::CFileHelper()
		: _locale(_tsetlocale(LC_ALL, _T(".ACP")))
	{
	}

	CFileHelper::~CFileHelper()
	{
		_tsetlocale(LC_ALL, _locale);
	}

	CCurrentDirectoryTempSwitcher::CCurrentDirectoryTempSwitcher(const TCHAR* directory)
	{
		_last_current_directory = GetCurrentDirectory();
		::SetCurrentDirectory(directory);
	}

	CCurrentDirectoryTempSwitcher::~CCurrentDirectoryTempSwitcher()
	{
		::SetCurrentDirectory(_last_current_directory);
	}

}