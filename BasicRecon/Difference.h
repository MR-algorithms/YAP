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
		IData * _reference_data;
	};
}

