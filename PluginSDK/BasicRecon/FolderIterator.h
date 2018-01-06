#pragma once

#ifndef _FolderIterator_h__20171227
#define _FolderIterator_h__20171227

#include "Implement\ProcessorImpl.h"

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

	private:
		~FolderIterator();

		void GetFolders(std::string path, std::vector<std::string>& folders, bool is_subfolder);

		std::string ToMbs(const wchar_t * wcs);
	};
}

#endif
