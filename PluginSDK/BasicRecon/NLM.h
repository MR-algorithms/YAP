#pragma once
#include "Implement\ProcessorImpl.h"
#include "Client/DataHelper.h"
#include <armadillo>

namespace Yap
{
	class NLM :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(NLM)
	public:
		NLM(void);
		NLM(const NLM& rhs);

	protected:
		~NLM();

		virtual bool Input(const wchar_t * name, IData * data) override;
		
		void nlmeans(float * input_img, float * output_img, unsigned int pl_r, 
			unsigned int sw_r, float sigma, float h, unsigned int width, unsigned int height);
		arma::fmat GetGaussianKernel(int pl, float sigma);

		float GetSigma(float * input_img, unsigned int width, unsigned int height);
	};
}
