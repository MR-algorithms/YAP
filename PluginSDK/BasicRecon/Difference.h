#pragma once

#ifndef Difference_h__20160816
#define Difference_h__20160816

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap 
{
	class CDifference :
		public ProcessorImpl
	{
	public:
		CDifference();
		virtual ~CDifference();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		SmartPtr<IData> _reference_data;
	};
}

#endif // Difference_h__
