#pragma once

#ifndef Grappa_h__20160814
#define Grappa_h__20160814

#include "Interface/Implement/ProcessorImpl.h"
#include "Interface/Client/DataHelper.h"
#include <complex>
#include <armadillo>

namespace Yap
{
	class Grappa :
		public ProcessorImpl
	{
	public:
		Grappa(void);
		Grappa(const Grappa& rhs);
		virtual ~Grappa();

		virtual bool Input(const wchar_t * port, IData * data) override;

		bool Recon(std::complex<float> * subsampled_data,
			size_t r, size_t acs, size_t Block, size_t width, size_t height, size_t Num_coil);

	protected:
		std::complex<float> * MakeFidelity(std::complex<float> * recon_data, std::vector<std::complex<float>> acs_data,
			size_t r, size_t acs, size_t width, size_t height, size_t num_coil);
		arma::cx_mat FitCoef(std::complex<float> * subsampled_data,
			size_t R, size_t acs, size_t Block, size_t Width, size_t height, size_t Num_coil);

		std::vector<std::complex<float>> GetAcsData(std::complex<float> * data, 
			size_t r, size_t acs, size_t width, size_t height, size_t num_coil);

		virtual IProcessor * Clone() override;

		// Inherited via ProcessorImpl
		virtual bool OnInit() override;
	};
}
#endif // Grappa_h__


