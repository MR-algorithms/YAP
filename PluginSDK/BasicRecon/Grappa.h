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
			unsigned int r, unsigned int acs, unsigned int Block, unsigned int width, unsigned int height, unsigned int num_coil);

	protected:
		std::complex<float> * MakeFidelity(std::complex<float> * recon_data, std::vector<std::complex<float>> acs_data,
			unsigned int r, unsigned int acs, unsigned int width, unsigned int height, unsigned int num_coil);
		 arma::cx_fmat FitCoef(std::complex<float> * subsampled_data,
		unsigned int R, unsigned int acs, unsigned int Block, unsigned int Width, unsigned int height, unsigned int Num_coil);

		std::vector<std::complex<float>> GetAcsData(std::complex<float> * data, 
			unsigned int r, unsigned int acs, unsigned int width, unsigned int height, unsigned int num_coil);

		virtual IProcessor * Clone() override;

		// Inherited via ProcessorImpl
		virtual bool OnInit() override;
	};
}
#endif // Grappa_h__


