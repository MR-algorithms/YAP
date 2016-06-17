#pragma once
#include "ProcessorImp.h"
#include "DataHelper.h"
#include "fftw3.h"
#include <complex>
#include <vector>
class CFft1D :
	public CProcessorImp
{
public:
	CFft1D();
	virtual ~CFft1D();

	virtual bool Init() override;

	virtual bool Input(const wchar_t * port, IData * data) override;

	virtual wchar_t * GetId() override;


	std::vector<std::complex<double>> Transform(const std::complex<double>* input);
	void FFTShift(std::vector<std::complex<double>>& data);

	void SetIFFTPlan(const fftw_plan& plan) { _fft_plan = plan; }

protected:

	void SwapBlock(std::complex<double> * block1, std::complex<double> * block2,
		unsigned int width);
	unsigned int _width;
	fftw_plan _fft_plan;
	bool Fft1D(IData* data);

};

