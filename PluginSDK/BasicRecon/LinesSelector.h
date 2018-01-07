#pragma once

#ifndef LINESSELECTOR_H_20171031
#define LINESSELECTOR_H_20171031

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class LinesSelector :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(LinesSelector)
	public:
		LinesSelector();
		LinesSelector(const LinesSelector& rhs);

	protected:
		virtual ~LinesSelector();
		virtual bool Input(const wchar_t * name, IData * data) override;
	};
}


#endif // !LINESSELECTOR_H_