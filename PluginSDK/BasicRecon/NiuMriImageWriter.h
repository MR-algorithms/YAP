#pragma once
#ifndef NiuMriImageWriter_h__
#define NiuMriImageWriter_h__

#include "Implement/processorImpl.h"

namespace Yap
{
	class NiuMriImageWriter :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(NiuMriImageWriter)
	public:
		NiuMriImageWriter(void);
		NiuMriImageWriter(const NiuMriImageWriter& rhs);

	private:
		~NiuMriImageWriter();
		virtual bool Input(const wchar_t * name, IData * data);
		std::wstring GetFilePath(const wchar_t * output_folder, const wchar_t * ouput_name);
	};
}

#endif // NiuMriImageWriter_h__
