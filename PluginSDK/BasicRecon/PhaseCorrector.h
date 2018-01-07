#pragma once

#ifndef PHASECORRECTOR_H_20171101
#define PHASECORRECTOR_H_20171101

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class PhaseCorrector :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(PhaseCorrector)
	public:
		PhaseCorrector();
		PhaseCorrector(const PhaseCorrector& rhs);

	protected:
		~PhaseCorrector();
		virtual bool Input(const wchar_t * name, IData * data) override;

		SmartPtr<IData> _phase;
	};

}
#endif // !PHASECORRECTOR_H_


