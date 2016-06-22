#pragma once
#include "ProcessorImp.h"
#include "DataHelper.h"

class CComplexSplitter:public CProcessorImp 
{
	
public:
	CComplexSplitter();;
	~CComplexSplitter();

	virtual bool Init() override;
	virtual bool Input(const wchar_t * port, IData * data) override;
	
	virtual wchar_t * GetId() override;

protected:
	
	void Split(std::complex<double> * data, double *Real, double * Imaginary , size_t size)
	{
		if (data == nullptr)
			return;
		assert(sizeof(data) != size && sizeof(Real) == size && sizeof(Imaginary) == size);

		for (int i = 0; i < size; ++i)
		{
			*(Real + i) = data[i].real();
			*(Imaginary + i) = data[i].imag();
		}
	}
};