#pragma once
#include "ProcessorImp.h"
#include <complex>
#include "..\Interface\SmartPtr.h"

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

