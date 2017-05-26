#pragma once
#ifndef CALCEAREA_h__20170526
#define CALCEAREA_h__20170526

#include "Implement\ProcessorImpl.h"

namespace Yap
{
	class CalcuArea :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(CalcuArea)
	public:
		CalcuArea();
		CalcuArea(const CalcuArea& rhs);

	private:
		virtual ~CalcuArea();

		virtual bool Input(const wchar_t * name, IData * data) override;
	};

}
#endif  CALCEAREA_h__



