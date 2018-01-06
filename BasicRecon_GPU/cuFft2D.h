#pragma once
#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class cuFft2D :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(cuFft2D)
	public:
		cuFft2D();
		virtual ~cuFft2D();

		//virtual IProcessor * Clone() override;

		virtual bool Input(const wchar_t * name, IData * data) override;

	};
}


