#pragma once

#include "Implement/ProcessorImpl.h"
#include "Client/DataHelper.h"
#include <fftw3.h>
#include <complex>
#include <vector>
#include<bitset>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <sstream>

const double pai = 3.1415926535897932384626433832795028841971; // Բ����

typedef std::vector<std::vector<std::complex<float>>> VVCD;
typedef std::vector<std::vector<float> >VVD;


namespace Yap
{
	class IcePeFt :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(IcePeFt)
	public:
		IcePeFt();
		IcePeFt(const IcePeFt& rhs);

	protected:
		~IcePeFt();

		virtual bool Input(const wchar_t * port, IData * data) override;

		void PeFtShift(std::complex<float>* data, size_t sidth, size_t height);
		void FftShift(std::complex<float>* data, size_t width, size_t height);
		void SwapBlock(std::complex<float> * block1, std::complex<float> * block2, size_t width, size_t height, size_t line_stride);
	};

}
class FFT1 {
private:
	unsigned N; // ���ݳ���

	unsigned NFFT; // ʹ�ÿ��ٸ���Ҷ�仯������ĳ���

	unsigned r; // ���ݳ���NFFT=2^r,��2^(r-1)<N<<NFFT

	std::vector<std::complex<float> >TS; // ʱ��������

	std::vector<std::complex<float> >FS; // Ƶ��������FrequencySeries

	std::vector<float>PSD; // PSD�������ܶ�(��ɢ��)

	std::vector<float>PS; // ����PS=PSD*Frequency
	int sign;

	// sign > 0Ϊ���任����Ϊ��任
	// ------------------------------------
	void Set_NFFT_r(unsigned n);
	unsigned RevBit(unsigned num);
	void GetIndex(unsigned L, unsigned ALindex, unsigned &AL_1index0,
		unsigned &AL_1index1, unsigned &Windex);

	// ------------------------------------

	void doFFT1();

	// ִ�и���Ҷ�任
	// ------------------------------------
public:

	// ------ʵ���й��캯��-------------
	FFT1(std::vector<float>TimeSeries, int sign_ = 1) {
		N = TimeSeries.size();
		if (sign_ > 0) {
			sign = 1.0; // ���任
		}
		else {
			sign = -1.0; // ��任
		}
		Set_NFFT_r(N);

		std::complex<float>W0(0.0, 0.0); // ��ת����
										 // --------------------------------
		for (unsigned i = 0; i < NFFT; i++) { // ����ֵ
			if (i < N) {
				FS.push_back(W0);
				std::complex<float>Wi(TimeSeries[i], 0.0); // ��ת����
				TS.push_back(Wi);
			}
			else {
				FS.push_back(W0); // ����
				TS.push_back(W0); // ����
			}

		}
		// --------------------------------

		doFFT1(); // ִ�и���Ҷ�任
				  // --------------------------------
	};

	// -------�����й��캯��-------------
	FFT1(std::vector<std::complex<float> >TimeSeries, int sign_ = 1) { // ����ֵ
		N = TimeSeries.size();
		if (sign_ > 0) {
			sign = 1.0; // ���任
		}
		else {
			sign = -1.0; // ��任
		}
		Set_NFFT_r(N);

		std::complex<float>W0(0.0, 0.0); // ��ת����
										 // --------------------------------
		for (unsigned i = 0; i < NFFT; i++) { // ����ֵ
			if (i < N) {
				FS.push_back(W0);
				TS.push_back(TimeSeries[i]);
			}
			else {
				FS.push_back(W0); // ����
				TS.push_back(W0); // ����
			}

		}
		// --------------------------------

		doFFT1(); // ִ�и���Ҷ�任
				  // --------------------------------
	};

	// ----------��Ա����------

	// ----------��Ա���� -------------
	std::vector<std::complex<float> >GetFS() { // ����Ƶ��������FrequencySeries
		return FS;
	};

	// ----------��Ա���� -------------
	std::vector<float>GetFrequency() { // ����Ƶ��
		std::vector<float>Frequency;
		// ----------------------
		for (unsigned int i = 0; i < FS.size(); i++) {
			Frequency.push_back(i);
		}
		return Frequency;
	};

	// ----------��Ա���� -------------
	std::vector<float>GetPSD() { // ����FS^2
		if (PSD.size() > 0) {
			PSD.clear();
		}
		// ----------------------
		for (unsigned int i = 0; i < FS.size(); i++) {
			PSD.push_back(real(FS[i] * conj(FS[i])));
		}
		return PSD;
	};

	std::vector<float>GetPS() { // ����PS=PSD*Frequency
		if (PS.size() > 0) {
			PS.clear();
		}
		// ----------------------
		for (unsigned int i = 0; i < FS.size(); i++) {
			PS.push_back(i * 1.0 * real(FS[i] * conj(FS[i])));
		}
		return PS;
	};
	// ----------�������� -------------

	~FFT1() {
	};
	// -----------------------------------
};

class FFT2 {

private:

	// ----------------------------------------------
	unsigned N1; // �����г��� 0<=k1<N1  0<=j1<N1

	unsigned N2; // �����г��� 0<=k2<N2  0<=j2<N2

	int sign; // sign > 0Ϊ���任����Ϊ��任

	VVCD Tk1k2; // ��άʱ��������,��k1��k2

	VVCD Yj1k2; // �м����ж� Ak1k2ÿһ��(k1)������Ҷ�任�Ľ��

	VVCD Fj1j2; // Ƶ��������FrequencySeries

	VVD PSD; // PSD�������ܶ�(��ɢ��)

	VVD PS; // ����PS=PSD*Frequency

			// ------------------------------------
	VVCD TranspositionVVCD(VVCD dv); // ����ת��
									 // ------------------------------------

public:

	// ------ʵ���й��캯��-------------
	FFT2(VVD TK1K2, int sign_ = 1) { // ����ֵ

		N1 = TK1K2.size(); // ��ȡ����
		Tk1k2.resize(N1);
		for (unsigned k1 = 0; k1 < N1; k1++) {
			N2 = TK1K2[k1].size();
			Tk1k2[k1].resize(N2);
			for (unsigned k2 = 0; k2 < N2; k2++) {
				std::complex<float> W(TK1K2[k1][k2], 0.0);
				Tk1k2[k1][k2] = W;
			}

		}
		// -----------------------------------------
		for (unsigned k1 = 0; k1 < N1; k1++) {
			FFT1 fft1(Tk1k2[k1], sign_);
			Yj1k2.push_back(fft1.GetFS());
		}
		// -----------------------------------------
		Yj1k2 = TranspositionVVCD(Yj1k2); // ������ת��
										  // -----------------------------------------
		N2 = Yj1k2.size(); // ��ȡ����
		for (unsigned k2 = 0; k2 < N2; k2++) {
			FFT1 fft1(Yj1k2[k2], sign_);
			Fj1j2.push_back(fft1.GetFS());
		}
		// --------------
		Fj1j2 = TranspositionVVCD(Fj1j2); // ������ת��
										  // -----------------------
	};

	// -------�����й��캯��-------------
	FFT2(VVCD TK1K2, int sign_ = 1) { // ����ֵ
		Tk1k2 = TK1K2;
		N1 = Tk1k2.size(); // ��ȡ����
		//for (unsigned k1 = 0; k1 < N1; k1++) {
		//	FFT1 fft1(Tk1k2[k1], sign_);
		//	Yj1k2.push_back(fft1.GetFS());
		//}
		// -----------------------------------------
		Yj1k2 = TranspositionVVCD(Tk1k2);
		//Yj1k2 = TranspositionVVCD(Yj1k2); // ������ת��
										  // -----------------------------------------
		N2 = Yj1k2.size(); // ��ȡ����
		for (unsigned k2 = 0; k2 < N2; k2++) {
			FFT1 fft1(Yj1k2[k2], sign_);
			Fj1j2.push_back(fft1.GetFS());
		}
		// --------------
		Fj1j2 = TranspositionVVCD(Fj1j2); // ������ת��
										  // -----------------------
	};

	// ----------��Ա���� -------------
	VVCD GetFS() { // ����Ƶ��������Fj1j2
		return Fj1j2;
	};

	// ----------��Ա���� -------------
	VVD GetPSD() { // ����FS^2
		PSD.resize(Fj1j2.size());
		for (unsigned i = 0; i < Fj1j2.size(); i++) {
			PSD[i].resize(Fj1j2[i].size());
		}
		// ----------------------
		for (unsigned i = 0; i < Fj1j2.size(); i++) {
			for (unsigned j = 0; j < Fj1j2[i].size(); j++) {
				PSD[i][j] = real(Fj1j2[i][j] * conj(Fj1j2[i][j]));
			}
		}
		return PSD;
	};

	// ----------��Ա���� -------------
	VVD GetPS() { // ����PS=PSD*Frequency
		PS.resize(Fj1j2.size());
		for (unsigned i = 0; i < Fj1j2.size(); i++) {
			PS[i].resize(Fj1j2[i].size());
		}
		// ----------------------
		for (unsigned i = 0; i < Fj1j2.size(); i++) {
			for (unsigned j = 0; j < Fj1j2[i].size(); j++) {
				PS[i][j] = (double)i * (double)j * real
					(Fj1j2[i][j] * conj(Fj1j2[i][j]));
			}
		}
		return PS;
	};
	// ----------�������� -------------

	~FFT2() {
	};
	// -----------------------------------
};

