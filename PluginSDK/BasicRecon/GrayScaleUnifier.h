#pragma once

#ifndef GRAYSCALEUNIFIER_H_20171102
#define GRAYSCALEUNIFIER_H_20171102

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class GrayScaleUnifier :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(GrayScaleUnifier)
	public:
		GrayScaleUnifier();
		GrayScaleUnifier(const GrayScaleUnifier& rhs);

	protected:
		~GrayScaleUnifier();
		virtual bool Input(const wchar_t * name, IData * data) override;

	};
}
#endif // !GRAYSCALEUNIFIER_H_



