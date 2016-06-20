#pragma once
#include "ProcessorImp.h"
#include <vector>

class CZeroFilling :
	public CProcessorImp
{
public:
	CZeroFilling();
	virtual ~CZeroFilling();

	virtual bool Input(const wchar_t * port, IData * data) override;

	virtual wchar_t * GetId() override;

protected:
	bool ZeroFilling(IData * input, unsigned int out_readout, unsigned int out_phase_encoding);

	std::vector<double> _output_image;

};

