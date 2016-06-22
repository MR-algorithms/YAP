#pragma once
#include "ProcessorImp.h"


class CModulePhase :
	public CProcessorImp
{
public:
	CModulePhase();
	virtual ~CModulePhase();

	virtual bool Input(const wchar_t * port, IData * data) override;

	virtual wchar_t * GetId() override;

protected:
	bool GetModule(std::complex<double>* input_data, double* module, 
		unsigned int width, unsigned int height);
	bool GetPhase(std::complex<double>* input_data, double* phase,
		unsigned int width, unsigned int height);

};

