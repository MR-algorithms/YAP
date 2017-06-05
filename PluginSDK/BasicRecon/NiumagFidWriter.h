#pragma once
#ifndef NiumagFidWriter_h__
#define NiumagFidWriter_h__

#include "Implement/processorImpl.h"

namespace Yap
{
	class NiumagFidWriter :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(NiumagFidWriter)
	public:
		NiumagFidWriter(void);
		NiumagFidWriter(const NiumagFidWriter& rhs);
		
	private:		
		~NiumagFidWriter();		
		
		virtual bool Input(const wchar_t * name, IData * data) override;
		std::wstring GetFilePath(const wchar_t * output_folder, const wchar_t * ouput_name);
	};
}

#endif // NiumagFidWriter_h__

