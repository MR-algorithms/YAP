#pragma once

#ifndef _FolderIterator_h__20171227
#define _FolderIterator_h__20171227

#include "Implement\ProcessorImpl.h"
#include <vector>

#ifndef _MAX_PATH
#define _MAX_PATH   260 // max. length of full pathname
#endif // !_MAX_PATH

#ifndef _MAX_FNAME
#define _MAX_FNAME  256 // max. length of file name component
#endif // !_MAX_FNAME


namespace Yap
{
	class FolderIterator : public ProcessorImpl {
		IMPLEMENT_SHARED(FolderIterator)
	public:
		FolderIterator();
		FolderIterator(const FolderIterator&);

		virtual bool Input(const wchar_t * name, IData * data) override;
		bool InputObsolete(const wchar_t * name, IData * data) ;

	private:
		~FolderIterator();

		std::vector<std::wstring> GetFolders(const wchar_t * path, bool recursive, const wchar_t * regex);

		std::string ToMbs(const wchar_t * wcs);
	};
}

#endif
