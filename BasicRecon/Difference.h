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

		virtual bool Input(const wchar_t * port, IData * dest_data) override;

	protected:
		bool SetStartData(const wchar_t * port, IData * start_data);
		IData * GetStartData();

		bool ResetStartData();

		bool Difference(double* start, double* dest, size_t size, double* output);

		IData * _start_data;
		bool _is_set_start;

	};

}

