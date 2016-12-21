#pragma once
#ifndef NiuMriImageWriter_h__
#define NiuMriImageWriter_h__

#include "Interface/Implement/processorImpl.h"

namespace Yap
{
	class NiuMriImageWriter :
		public ProcessorImpl
	{
	public:
		NiuMriImageWriter(void);
		NiuMriImageWriter(const NiuMriImageWriter& rhs);

	private:
		~NiuMriImageWriter();

		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * name, IData * data);

		std::wstring GetFilePath(const wchar_t * output_folder, const wchar_t * output_name);
	};

}

#endif // NiuMriImageWriter_h__
