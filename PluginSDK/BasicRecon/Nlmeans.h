#pragma once
#include "Interface\Implement\ProcessorImpl.h"
#include <armadillo>

#define MAX(i, j) ((i) < (j) ? (j) : (i))
#define MIN(i, j) ((i) < (j) ? (i) : (j))

#define dTiny 1e-10
#define fTiny 0.00000001f
#define fLarge 100000000.0f

#define LUTMAX 30.0
#define LUTMAXM1 29.0
#define LUTPRECISION 1000.0

namespace Yap
{
	class Nlmeans :
		public ProcessorImpl
	{
	public:
		Nlmeans(void);
		Nlmeans(const Nlmeans& rhs);
		virtual ~Nlmeans();

		virtual IProcessor * Clone() override;
		virtual bool Input(const wchar_t * name, IData * data) override;

	protected:
		void nlmeans_ipol(unsigned int iDWin, unsigned int iDBloc, float Sigma, float fFiltPar, 
			float * fpI, float * fpO, unsigned int iWidth, unsigned int iHeight);

		float fiL2FloatDist(float * u0, float *u1, unsigned int i0, unsigned int j0, 
			unsigned int i1, unsigned int j1, int radius, unsigned int width0, unsigned int width1);

		void  wxFillExpLut(float *lut, unsigned int size);

		float wxSLUT(float dif, float *lut);

		float GetSigma(float * input_img, unsigned int width, unsigned int height);
	};
}
