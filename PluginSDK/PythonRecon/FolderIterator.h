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
		
	private:
		~FolderIterator();

		void GetFolders(std::vector<std::wstring>& folder,std::wstring path, bool recursive, std::wstring regex);

		void NotifyIterationFinished();

	};
}

#endif
