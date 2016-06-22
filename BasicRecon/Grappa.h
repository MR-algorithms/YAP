#pragma once
#include "ProcessorImp.h"
#include "DataHelper.h"
#include <complex>
#include <vector>
#include <armadillo>

class CGrappa :
	public CProcessorImp
{
public:
	CGrappa();
	virtual ~CGrappa();

	virtual bool Init() override;

	virtual bool Input(const wchar_t * port, IData * data) override;

	virtual wchar_t * GetId() override;

	bool Recon(std::complex<double> * subsampled_data, 
		size_t r, size_t acs, size_t Block, size_t width, size_t height, size_t Num_coil);


protected:

	std::complex<double> * MakeFidelity(std::complex<double> * recon_data, std::vector<std::complex<double>> acs_data, 
		size_t r, size_t acs, size_t width, size_t height, size_t num_coil);
	arma::cx_mat FitCoef(std::complex<double> * subsampled_data, 
		size_t R, size_t acs, size_t Block, size_t Width, size_t height, size_t Num_coil);

	std::vector<std::complex<double>> GetAcsData(std::complex<double> * data, size_t r, size_t acs, size_t width, size_t height, size_t num_coil);


};

