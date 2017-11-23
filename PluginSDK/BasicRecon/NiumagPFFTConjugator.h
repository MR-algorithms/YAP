#pragma once

#ifndef CONJUGATOR_H_20171101
#define CONJUGATOR_H_20171101

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class NiumagPFFTConjugator :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(NiumagPFFTConjugator)
	public:
		NiumagPFFTConjugator();
		NiumagPFFTConjugator(const NiumagPFFTConjugator& rhs);

	protected:
		~NiumagPFFTConjugator();
		virtual bool Input(const wchar_t * name, IData * data) override;
	};
}
#endif // !CONJUGATOR_H_



