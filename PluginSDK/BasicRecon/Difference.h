#pragma once

#ifndef Difference_h__20160816
#define Difference_h__20160816

#include "Interface/Implement/ProcessorImp.h"
#include "Interface/SmartPtr.h"

namespace Yap 
{
	class CDifference :
		public CProcessorImp
	{
	public:
		CDifference();
		virtual ~CDifference();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		CSmartPtr2<IData, IMemory> _reference_data;
	};
}

#endif // Difference_h__
