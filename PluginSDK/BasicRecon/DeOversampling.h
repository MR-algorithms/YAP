#pragma once

#ifndef DeOversampling_h__20160814
#define DeOversampling_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class DeOversampling :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(DeOversampling)
	public:
		DeOversampling();
		DeOversampling(const DeOversampling& rhs);

	protected:
		~DeOversampling();
		virtual bool Input(const wchar_t * port, IData * data) override;
		bool FreqInput(const wchar_t * port, IData * data);
		bool ProcessOverSampling(const wchar_t * port, IData * data);
		
	};
}
#endif // DeOversampling_h__
