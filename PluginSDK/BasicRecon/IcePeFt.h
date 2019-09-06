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

const double pai = 3.1415926535897932384626433832795028841971; // 圆周率

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
	unsigned N; // 数据长度

	unsigned NFFT; // 使用快速傅里叶变化所计算的长度

	unsigned r; // 数据长度NFFT=2^r,且2^(r-1)<N<<NFFT

	std::vector<std::complex<float> >TS; // 时间域序列

	std::vector<std::complex<float> >FS; // 频率域序列FrequencySeries

	std::vector<float>PSD; // PSD功率谱密度(离散谱)

	std::vector<float>PS; // 返回PS=PSD*Frequency
	int sign;

	// sign > 0为正变换否则为逆变换
	// ------------------------------------
	void Set_NFFT_r(unsigned n);
	unsigned RevBit(unsigned num);
	void GetIndex(unsigned L, unsigned ALindex, unsigned &AL_1index0,
		unsigned &AL_1index1, unsigned &Windex);

	// ------------------------------------

	void doFFT1();

	// 执行傅里叶变换
	// ------------------------------------
public:

	// ------实序列构造函数-------------
	FFT1(std::vector<float>TimeSeries, int sign_ = 1) {
		N = TimeSeries.size();
		if (sign_ > 0) {
			sign = 1.0; // 正变换
		}
		else {
			sign = -1.0; // 逆变换
		}
		Set_NFFT_r(N);

		std::complex<float>W0(0.0, 0.0); // 旋转因子
										 // --------------------------------
		for (unsigned i = 0; i < NFFT; i++) { // 赋初值
			if (i < N) {
				FS.push_back(W0);
				std::complex<float>Wi(TimeSeries[i], 0.0); // 旋转因子
				TS.push_back(Wi);
			}
			else {
				FS.push_back(W0); // 补零
				TS.push_back(W0); // 补零
			}

		}
		// --------------------------------

		doFFT1(); // 执行傅里叶变换
				  // --------------------------------
	};

	// -------复序列构造函数-------------
	FFT1(std::vector<std::complex<float> >TimeSeries, int sign_ = 1) { // 赋初值
		N = TimeSeries.size();
		if (sign_ > 0) {
			sign = 1.0; // 正变换
		}
		else {
			sign = -1.0; // 逆变换
		}
		Set_NFFT_r(N);

		std::complex<float>W0(0.0, 0.0); // 旋转因子
										 // --------------------------------
		for (unsigned i = 0; i < NFFT; i++) { // 赋初值
			if (i < N) {
				FS.push_back(W0);
				TS.push_back(TimeSeries[i]);
			}
			else {
				FS.push_back(W0); // 补零
				TS.push_back(W0); // 补零
			}

		}
		// --------------------------------

		doFFT1(); // 执行傅里叶变换
				  // --------------------------------
	};

	// ----------成员函数------

	// ----------成员函数 -------------
	std::vector<std::complex<float> >GetFS() { // 返回频率域序列FrequencySeries
		return FS;
	};

	// ----------成员函数 -------------
	std::vector<float>GetFrequency() { // 返回频率
		std::vector<float>Frequency;
		// ----------------------
		for (unsigned int i = 0; i < FS.size(); i++) {
			Frequency.push_back(i);
		}
		return Frequency;
	};

	// ----------成员函数 -------------
	std::vector<float>GetPSD() { // 返回FS^2
		if (PSD.size() > 0) {
			PSD.clear();
		}
		// ----------------------
		for (unsigned int i = 0; i < FS.size(); i++) {
			PSD.push_back(real(FS[i] * conj(FS[i])));
		}
		return PSD;
	};

	std::vector<float>GetPS() { // 返回PS=PSD*Frequency
		if (PS.size() > 0) {
			PS.clear();
		}
		// ----------------------
		for (unsigned int i = 0; i < FS.size(); i++) {
			PS.push_back(i * 1.0 * real(FS[i] * conj(FS[i])));
		}
		return PS;
	};
	// ----------析构函数 -------------

	~FFT1() {
	};
	// -----------------------------------
};

class FFT2 {

private:

	// ----------------------------------------------
	unsigned N1; // 数据行长度 0<=k1<N1  0<=j1<N1

	unsigned N2; // 数据列长度 0<=k2<N2  0<=j2<N2

	int sign; // sign > 0为正变换否则为逆变换

	VVCD Tk1k2; // 二维时间域序列,行k1列k2

	VVCD Yj1k2; // 中间序列对 Ak1k2每一行(k1)做傅里叶变换的结果

	VVCD Fj1j2; // 频率域序列FrequencySeries

	VVD PSD; // PSD功率谱密度(离散谱)

	VVD PS; // 返回PS=PSD*Frequency

			// ------------------------------------
	VVCD TranspositionVVCD(VVCD dv); // 矩阵转置
									 // ------------------------------------

public:

	// ------实序列构造函数-------------
	FFT2(VVD TK1K2, int sign_ = 1) { // 赋初值

		N1 = TK1K2.size(); // 获取行数
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
		Yj1k2 = TranspositionVVCD(Yj1k2); // 将矩阵转置
										  // -----------------------------------------
		N2 = Yj1k2.size(); // 获取列数
		for (unsigned k2 = 0; k2 < N2; k2++) {
			FFT1 fft1(Yj1k2[k2], sign_);
			Fj1j2.push_back(fft1.GetFS());
		}
		// --------------
		Fj1j2 = TranspositionVVCD(Fj1j2); // 将矩阵转置
										  // -----------------------
	};

	// -------复序列构造函数-------------
	FFT2(VVCD TK1K2, int sign_ = 1) { // 赋初值
		Tk1k2 = TK1K2;
		N1 = Tk1k2.size(); // 获取行数
		//for (unsigned k1 = 0; k1 < N1; k1++) {
		//	FFT1 fft1(Tk1k2[k1], sign_);
		//	Yj1k2.push_back(fft1.GetFS());
		//}
		// -----------------------------------------
		Yj1k2 = TranspositionVVCD(Tk1k2);
		//Yj1k2 = TranspositionVVCD(Yj1k2); // 将矩阵转置
										  // -----------------------------------------
		N2 = Yj1k2.size(); // 获取列数
		for (unsigned k2 = 0; k2 < N2; k2++) {
			FFT1 fft1(Yj1k2[k2], sign_);
			Fj1j2.push_back(fft1.GetFS());
		}
		// --------------
		Fj1j2 = TranspositionVVCD(Fj1j2); // 将矩阵转置
										  // -----------------------
	};

	// ----------成员函数 -------------
	VVCD GetFS() { // 返回频率域序列Fj1j2
		return Fj1j2;
	};

	// ----------成员函数 -------------
	VVD GetPSD() { // 返回FS^2
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

	// ----------成员函数 -------------
	VVD GetPS() { // 返回PS=PSD*Frequency
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
	// ----------析构函数 -------------

	~FFT2() {
	};
	// -----------------------------------
};

