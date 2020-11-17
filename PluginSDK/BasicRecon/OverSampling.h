#pragma once

#ifndef OverSampling_h__20160814
#define OverSampling_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class OverSampling :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(OverSampling)
	public:
		OverSampling();
		OverSampling(const OverSampling& rhs);

	protected:
		~OverSampling();
		virtual bool Input(const wchar_t * port, IData * data) override;
		
	};
}
#endif // OverSampling_h__
