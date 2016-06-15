#pragma once
#include "ProcessorImp.h"
#include "DataHelper.h"
#include "fftw3.h"
#include <complex>
#include <vector>

class CFft2D :
	public CProcessorImp
{
public:
	CFft2D();
	virtual ~CFft2D();

	virtual bool Init() override;

	virtual bool Input(const wchar_t * port, IData * data) override;

	virtual wchar_t * GetId() override;

	void SetFFTPlan(const fftw_plan& plan);
	std::vector<std::complex<double>> Transform(std::vector<std::complex<double>> input);
	void FftShift(std::vector<std::complex<double>>& data, unsigned int width, unsigned int height);

protected:
	void SwapBlock(std::complex<double> * block1, std::complex<double> * block2, unsigned int width, unsigned int height, unsigned int line_stride);

	fftw_plan _p_FFT;

	bool Fft2D(IData * data);

};

