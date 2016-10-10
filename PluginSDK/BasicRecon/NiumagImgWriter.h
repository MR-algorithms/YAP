#pragma once
#ifndef NiumagImgWriter_h__
#define NiumagImgWriter_h__

#include "Interface/Implement/processorImpl.h"

namespace Yap
{
	class NiumagImgWriter :
		public ProcessorImpl
	{
	public:
		NiumagImgWriter(void);
		NiumagImgWriter(const NiumagImgWriter& rhs);

	private:
		virtual bool Input(const wchar_t * name, IData * data);
		virtual IProcessor * Clone() override;

		virtual bool OnInit() override;
	};

}

#endif // NiumagImgWriter_h__
