#pragma once

#ifndef FilesIterator_h__20171228
#define FilesIterator_h__20171228

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class FilesIterator : public ProcessorImpl
	{
		IMPLEMENT_SHARED(FilesIterator)
	public:
		FilesIterator();

		FilesIterator(const FilesIterator &);
		
		bool Input(const wchar_t * name, IData * data);

	private:
		~FilesIterator();

		void NotifyIterationFinished(IData * data);

		void GetFiles(std::vector<std::wstring>& folders, std::wstring path, bool is_subfolder, std::wstring regular_exp);

	};
}

#endif
