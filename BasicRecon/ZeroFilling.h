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
	bool ZeroFilling(std::complex<double>* dest, unsigned int dest_width, unsigned int dest_height,
		std::complex<double>* source, unsigned int source_width, unsigned int source_height);
};

