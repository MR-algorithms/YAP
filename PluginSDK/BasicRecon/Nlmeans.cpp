#include "stdafx.h"
#include "Nlmeans.h"
#include "Client/DataHelper.h"
#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"

using namespace std;
using namespace Yap;
using namespace arma;

Nlmeans::Nlmeans(void):
	ProcessorImpl(L"Nlmeans")
{
	LOG_TRACE(L"Nlmeans constructor called.", L"BasicRecon");
	AddInput(L"Input", 2, DataTypeFloat);
	AddOutput(L"Output", 2, DataTypeFloat);
}

Yap::Nlmeans::Nlmeans(const Nlmeans & rhs) :
	ProcessorImpl(rhs)
{
	LOG_TRACE(L"Nlmeans constructor called.", L"BasicRecon");
}

Nlmeans::~Nlmeans()
{
	LOG_TRACE(L"Nlmeans destructor called.", L"BasicRecon");
}

bool Yap::Nlmeans::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && Yap::GetDataArray<float>(data) != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	if (data->GetDataType() != DataTypeFloat)
		return false;

	DataHelper input_data(data);
	unsigned int width = input_data.GetWidth();
	unsigned int height = input_data.GetHeight();

	float * fpI = GetDataArray<float>(data);

	auto fpO = CreateData<float>(data);

//	float Sigma = GetFloat(L"Sigma");
	float Sigma = GetSigma(fpI, width, height);

	unsigned int bloc, win;
	float fFiltPar;

	if (Sigma > 0.0f && Sigma <= 15.0f) {
		win = 1;
		bloc = 10;
		fFiltPar = 0.4f;

	}
	else if (Sigma > 15.0f && Sigma <= 30.0f) {
		win = 2;
		bloc = 10;
		fFiltPar = 0.4f;

	}
	else if (Sigma > 30.0f && Sigma <= 45.0f) {
		win = 3;
		bloc = 17;
		fFiltPar = 0.35f;

	}
	else if (Sigma > 45.0f && Sigma <= 75.0f) {
		win = 4;
		bloc = 17;
		fFiltPar = 0.35f;

	}
	else if (Sigma <= 100.0f) {

		win = 5;
		bloc = 17;
		fFiltPar = 0.30f;
	}
	else
	{
		return false;
	}

	nlmeans_ipol(win, bloc, Sigma, fFiltPar, fpI, GetDataArray<float>(fpO.get()), width, height);

	Feed(L"Output", fpO.get());

	return true;
}

void Yap::Nlmeans::nlmeans_ipol(unsigned int iDWin, unsigned int iDBloc, float Sigma, 
	float fFiltPar, float * fpI, float * fpO, unsigned int iWidth, unsigned int iHeight)
{
	unsigned int iwxh = iWidth * iHeight;
	unsigned int ihwl = (2 * iDWin + 1);
	unsigned int iwl = (2 * iDWin + 1) * (2 * iDWin + 1);

	float Sigma2 = Sigma * Sigma;
	float fH = fFiltPar * Sigma;
	float fH2 = fH * fH;
	fH2 *= (float)iwl;

	unsigned int iLutLength = (unsigned int)rintf((float)LUTMAX * (float)LUTPRECISION);
	float * fpLut = new float[iLutLength];//
	wxFillExpLut(fpLut, iLutLength);

	float * fpCount = new float[iwxh];//

	for (unsigned int y = 0; y < iHeight; y++)
	{
		for (unsigned int x = 0; x < iWidth; x++)
		{
			int iDWin0 = MIN(iDWin, MIN(iWidth - 1 - x, MIN(iHeight - 1 - y, MIN(x, y))));
			int imin = MAX(x - iDBloc, iDWin0);
			int jmin = MAX(y - iDBloc, iDWin0);

			int imax = MIN(x + iDBloc, iWidth - 1 - iDWin0);
			int jmax = MIN(y + iDBloc, iHeight - 1 - iDWin0);

			float fMaxWeight = 0.0f;

			float fTotalWeight = 0.0f;

			for (int j = jmin; j <= jmax; j++)
			{
				for (int i = imin; i <= imax; i++)
				{
					if (i != x || j != y)
					{
						float fDif = fiL2FloatDist(fpI, fpI, x, y, i, j, iDWin0, iWidth, iHeight);
						fDif = MAX(fDif - 2.0f * (float)iwl * Sigma2, 0.0f);
						fDif = fDif / fH2;

						float fWeight = wxSLUT(fDif, fpLut);

						if (fWeight > fMaxWeight)
						{
							fMaxWeight = fWeight;
						}
						fTotalWeight += fWeight;

						for (int is = -iDWin0; is <= iDWin0; is++)
						{
							int aiindex = (iDWin + is) * ihwl + iDWin;
							int ail = (j + is) * iWidth + i;
							for (int ir = -iDWin0; ir <= iDWin0; ir++)
							{
								int iindex = aiindex + ir;
								int il = ail + ir;
								fpO[iindex] += fWeight * fpI[il];
							}
						}
					}

					for (int is = -iDWin0; is <= iDWin0; is++)
					{
						int aiindex = (iDWin + is) * ihwl + iDWin;
						int ail = (y + is) * iWidth + x;
						for (int ir = -iDWin0; ir <= iDWin0; ir++)
						{
							int iindex = aiindex + ir;
							int il = ail + ir;
							fpO[iindex] += fMaxWeight * fpI[il];
						}
					}
				}
			}

			fTotalWeight += fMaxWeight;

			if (fTotalWeight > fTiny)
			{
				for (int is = -iDWin0; is <= iDWin0; is++)
				{
					int aiindex = (iDWin + is) * ihwl + iDWin;
					int ail = (y + is)*iWidth + x;
					for (int ir = -iDWin0; ir <= iDWin0; ir++)
					{
						int iindex = aiindex + ir;
						int il = ail + ir;

						fpCount[il]++;

						fpO[il] += fpO[iindex] / fTotalWeight;
					}
				}
			}
		}
	}

	for (int ii = 0; ii < iwxh; ii++)
	{
		if (fpCount[ii] > 0.0)
		{
			fpO[ii] /= fpCount[ii];
		}
		else
		{
			fpO[ii] = fpI[ii];
		}
	}

	delete[] fpLut;
	delete[] fpCount;
}

float Yap::Nlmeans::fiL2FloatDist(float * u0, float *u1, unsigned int i0, 
	unsigned int j0, unsigned int i1, unsigned int j1, int radius, unsigned int width0, unsigned int width1)
{
	float dist = 0.0;
	for (int s = -radius; s <= radius; s++) {

		int l = (j0 + s)*width0 + (i0 - radius);
		float *ptr0 = &u0[l];

		l = (j1 + s)*width1 + (i1 - radius);
		float *ptr1 = &u1[l];

		for (int r = -radius; r <= radius; r++, ptr0++, ptr1++) {
			float dif = (*ptr0 - *ptr1);
			dist += (dif*dif);
		}

	}

	return dist;
}

void Yap::Nlmeans::wxFillExpLut(float *lut, unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
	{
		lut[i] = expf(-(float)i / LUTPRECISION);
	}
}

float Yap::Nlmeans::wxSLUT(float dif, float *lut)
{
	if (dif >= (float)LUTMAXM1) return 0.0;

	unsigned int  x = (unsigned int)floor((double)dif * (float)LUTPRECISION);

	float y1 = lut[x];
	float y2 = lut[x + 1];

	return float(y1 + (y2 - y1)*(dif*LUTPRECISION - x));
}

float Yap::Nlmeans::GetSigma(float * input_img, unsigned int width, unsigned int height)
{
	vector<float> input_data(width * height);
	fmat input(height , width);
	input.zeros();
	memcpy(input_data.data(), input_img, width * height * sizeof(float));
	for (unsigned int row = 0; row < height; ++row)
	{
		for (unsigned int col = 0; col < width; ++col)
		{
			input(row, col) = input_data[row * width + col];
		}
	}

	fmat back_ground(20, 20);
	back_ground.zeros();
	back_ground(span(0, 9), span(0, 9)) = input(span(0, 9), span(0, 9));
	back_ground(span(0, 9), span(10, 19)) = input(span(0, 9), span((width - 10), width - 1));
	back_ground(span(10, 19), span(0, 9)) = input(span(height - 10, height - 1), span(0, 9));
	back_ground(span(10, 19), span(10, 19)) = input(span(height - 10, height - 1), span(width - 10, width - 1));

	auto sigma = stddev(vectorise(back_ground));
	return sigma;
}
