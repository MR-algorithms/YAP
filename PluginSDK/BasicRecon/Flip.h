#pragma once

#ifndef Flip_h__20160814
#define Flip_h__20160814

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class Flip :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(Flip)
	public:
		Flip();
		Flip(const Flip& rhs);

	protected:
		~Flip();
		virtual bool Input(const wchar_t * port, IData * data) override;
		
	};
}
#endif // Flip_h__
