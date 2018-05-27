#pragma once

#ifndef FileIterator_h__20171228
#define FileIterator_h__20171228

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class FileIterator : public ProcessorImpl
	{
		IMPLEMENT_SHARED(FileIterator)
	public:
		FileIterator();

		FileIterator(const FileIterator &);
		
		bool Input(const wchar_t * name, IData * data);

	private:
		~FileIterator();

		void NotifyIterationFinished(IData * data);

		void GetFiles(std::vector<std::wstring>& folders, std::wstring path, bool is_subfolder, std::wstring regular_exp);

	};
}

#endif // !FileIterator_h__20171228
