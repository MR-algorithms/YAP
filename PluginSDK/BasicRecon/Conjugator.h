#pragma once

#ifndef CONJUGATOR_H_20171101
#define CONJUGATOR_H_20171101

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class Conjugator :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(Conjugator)
	public:
		Conjugator();
		Conjugator(const Conjugator& rhs);

	protected:
		~Conjugator();
		virtual bool Input(const wchar_t * name, IData * data) override;
	};
}
#endif // !CONJUGATOR_H_



