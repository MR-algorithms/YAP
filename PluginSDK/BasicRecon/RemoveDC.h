#pragma once

#ifndef RemoveDC_h__20160814
#define RemoveDC_h__20160814

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class DcRemover :
		public ProcessorImpl
	{
	public:
		DcRemover();
		~DcRemover();

		virtual bool Input(const wchar_t * port, IData * data) override;

		virtual IProcessor * Clone() override;


		// Inherited via ProcessorImpl
		virtual bool OnInit() override;

	};
}

#endif // RemoveDC_h__
