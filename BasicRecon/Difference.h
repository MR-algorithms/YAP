#pragma once
#include "ProcessorImp.h"
#include <complex>

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
		bool SetReferenceData(const wchar_t * port, IData * reference_data);
		IData * GetReferenceData();
		
		bool ResetReferenceData();
		
		IData * _reference_data;
		bool _is_reference;
	};
}

