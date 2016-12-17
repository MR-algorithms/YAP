#pragma once
#include "Interface\Implement\ProcessorImpl.h"
#include <cusp/complex.h>
#include <thrust\device_vector.h>
#include <thrust\device_ptr.h>
#include <cufft.h>

namespace Yap
{
	class cuFft2D :
		public ProcessorImpl
	{
	public:
		cuFft2D();
		virtual ~cuFft2D();

		virtual IProcessor * Clone() override;

		virtual bool Input(const wchar_t * name, IData * data) override;

	};
}


