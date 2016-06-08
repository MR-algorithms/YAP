#pragma once
#include "ProcessorImp.h"
class CDummyProcessor :
	public CProcessorImp
{
public:
	CDummyProcessor();
	virtual ~CDummyProcessor();

	virtual wchar_t * GetId() override;

	virtual bool Init() override;

	virtual bool Input(const wchar_t * port, IData * data) override;
};

