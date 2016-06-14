#pragma once
#include <string>

#pragma comment(lib, "comsuppw.lib")

namespace FileSystem
{
	class CCurrentDirectoryTempSwitcher
	{
	public:
		CCurrentDirectoryTempSwitcher(const TCHAR* directory);
		~CCurrentDirectoryTempSwitcher();
	protected:
		CString _last_current_directory;
	};

	/// Declare an instance of this class to enable fstream to open files with chinese characters in path.
	class CFileHelper
	{
	public:
		CFileHelper();
		~CFileHelper();

		CString _locale;
	};

	bool IsDirectoryEmpty(const CString& directory);
	bool DeleteMultiFile(const CString& directory, const CString& filename);
	bool DeleteDirectory(const CString& directory, bool bDeleteItself = true);
	bool CopyDirectory(const CString& source, const CString& destination, bool force_uppercase_dest_path = false);

	bool CreateFolder(const CString& folder);
	bool CreateFolderTree(const CString& path);
	long GetFolderSize(const CString& folder);	// 得到目录中所含文件的总容量

	bool FileExists(const CString& filename);
	bool IsFileReadonly(const CString& filepath);

	CString GetCurrentDirectory();

	CString GetFolderFromPath(const CString& file);
	CString GetFileNameFromPath(const CString& path);
	CString GetFileExtensionFromPath(const CString& path);

	int GetDiskFreeSpace(LPCSTR pszDrive);
	bool IsValidPath(const CString& path);
	bool IsFolderEqual(const CString& dir1, const CString& dir2);

	CString FindFileInDirectory(const CString& directory_path, const CString& file_name, bool include_subdirectory = false);

	CString GetStartPath();

	CString GetTempFilePath(const CString& prefix);

	template <class Processor>
	void ForEachFile(const TCHAR* directory_path, const TCHAR* filename, 
		bool include_subdirectory, Processor processor)
	{
		CString file_name(filename);
		CString path(directory_path);
		path += _T("\\");
		path +=  (file_name.IsEmpty() ? TEXT("*.*") : file_name);

		CFileFind finder;
		bool working = (finder.FindFile(path) == TRUE);
		while (working)
		{
			working = (finder.FindNextFile() == TRUE);
			if (!finder.IsDirectory())
			{
				processor(finder.GetFilePath());
			}
		}

		if (include_subdirectory)
		{
			CFileFind folder_finder;
			bool folder_finder_working = (folder_finder.FindFile(CString(directory_path) + _T("\\*.*")) == TRUE);
			while (folder_finder_working)
			{
				folder_finder_working = (folder_finder.FindNextFile() == TRUE);
				if (folder_finder.IsDots())
				{
					continue;
				}
				else if (folder_finder.IsDirectory())
				{
					ForEachFile(folder_finder.GetFilePath(), file_name, true, processor);
				}
			}
		}
	}

#ifndef NO_BOOST
	CString SimplifyPath(const CString& path);
#endif
};

