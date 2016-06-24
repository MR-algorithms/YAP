#pragma once
#include "ProcessorImp.h"

namespace Yap
{
	class CModulePhase :
		public CProcessorImp
	{
	public:
		CModulePhase();
		virtual ~CModulePhase();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:
		bool GetModule(std::complex<double>* input_data, double* module,
			size_t size);
		bool GetPhase(std::complex<double>* input_data, double* phase,
			size_t size);

	};
}
