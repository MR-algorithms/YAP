#pragma once

#ifndef Grappa_h__20160814
#define Grappa_h__20160814

#include "Interface/Implement/ProcessorImpl.h"
#include "Interface/Client/DataHelper.h"
#include <complex>
#include <armadillo>

namespace Yap
{
	class CGrappa :
		public ProcessorImpl
	{
	public:
		CGrappa();
		virtual ~CGrappa();

		virtual bool Input(const wchar_t * port, IData * data) override;

		bool Recon(std::complex<double> * subsampled_data,
			size_t r, size_t acs, size_t Block, size_t width, size_t height, size_t Num_coil);

	protected:
		std::complex<double> * MakeFidelity(std::complex<double> * recon_data, std::vector<std::complex<double>> acs_data,
			size_t r, size_t acs, size_t width, size_t height, size_t num_coil);
		arma::cx_mat FitCoef(std::complex<double> * subsampled_data,
			size_t R, size_t acs, size_t Block, size_t Width, size_t height, size_t Num_coil);

		std::vector<std::complex<double>> GetAcsData(std::complex<double> * data, 
			size_t r, size_t acs, size_t width, size_t height, size_t num_coil);

		virtual IProcessor * Clone() override;
	};
}
#endif // Grappa_h__


