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

		void GetFiles(std::string path, std::vector<std::string>& folders, bool is_subfolder);

		std::string ToMbs(const wchar_t * wcs);
	};
}

#endif
