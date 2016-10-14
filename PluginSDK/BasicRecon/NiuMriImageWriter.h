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
		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;
	};

}

#endif // NiuMriImageWriter_h__
