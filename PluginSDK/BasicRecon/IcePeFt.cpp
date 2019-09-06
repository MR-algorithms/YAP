#include "stdafx.h"
#include "IcePeFt.h"

#include "Client/DataHelper.h"
#include "Implement/LogUserImpl.h"

#include <string>
#pragma comment(lib, "libfftw3-3.lib")
#pragma comment(lib, "libfftw3f-3.lib")
#pragma comment(lib, "libfftw3l-3.lib")
using namespace std;
using namespace Yap;

IcePeFt::IcePeFt() :
	ProcessorImpl(L"IcePeFt")
{
	AddProperty<bool>(L"Inverse", false, L"The direction of FFT2D.");
	AddProperty<bool>(L"InPlace", true, L"The position of FFT2D.");

	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);
}


IcePeFt::IcePeFt(const IcePeFt& rhs)
	:ProcessorImpl(rhs)
{
}


IcePeFt::~IcePeFt()
{
}

bool IcePeFt::Input(const wchar_t * port, IData * data)
{
	// Do some check.
	if (data == nullptr)
	{
		LOG_ERROR(L"<Fft2D> Invalid input data!", L"BasicRecon");
		return false;
	}
	if (_wcsicmp(port, L"Input") != 0)
	{
		LOG_ERROR(L"<Fft2D> Error input port name!", L"BasicRecon");
		return false;
	}

	DataHelper input_data(data);
	if (input_data.GetActualDimensionCount() != 2)
	{
		LOG_ERROR(L"<Fft2D> Error input data dimention!(2D data is available)!", L"BasicRecon");
		return false;
	}
	if (input_data.GetDataType() != DataTypeComplexFloat)
	{
		LOG_ERROR(L"<Fft2D> Error input data type!(DataTypeComplexFloat is available)!", L"BasicRecon");
		return false;
	}

	LOG_TRACE(L"<Fft2D> Input::After Check.", L"BasicRecon");

	auto width = input_data.GetWidth();
	auto height = input_data.GetHeight();

	auto data_array = GetDataArray<complex<float>>(data);
	
	VVCD my_data;
	my_data.resize(height);
	for (int i = 0; i < height;i++)
	{
		my_data[i].resize(width);
		for (int j = 0; j < width;j++)
		{
			my_data[i][j] = data_array[i*width + j];
		}
	}

	FFT2 my_fft(my_data);
	VVCD fft_array =my_fft.GetFS();
	for (int i = 0; i < height;i++)
	{
		for (int j = 0; j < width;j++)
		{
			data_array[i*width + j] = fft_array[i][j];
		}
	}

	//FftShift(data_array, width, height);
	PeFtShift(data_array, width, height);
	return Feed(L"Output", data);
}

void IcePeFt::PeFtShift(std::complex<float>* data, size_t width, size_t height)
{
	SwapBlock(data, data + height / 2 * width, width / 2, height / 2, width);
	SwapBlock(data + width / 2, data + height / 2 * width + width / 2, width / 2, height / 2, width);
}

void IcePeFt::FftShift(std::complex<float>* data, size_t  width, size_t height)
{
	LOG_TRACE(L"<Fft2D> FftShift::Before SwapBlock().", L"BasicRecon");
	SwapBlock(data, data + height / 2 * width + width / 2, width / 2, height / 2, width);
	SwapBlock(data + width / 2, data + height / 2 * width, width / 2, height / 2, width);
	LOG_TRACE(L"<Fft2D> FftShift::After SwapBlock().", L"BasicRecon");
}

void IcePeFt::SwapBlock(std::complex<float>* block1, std::complex<float>* block2, size_t width, size_t height, size_t line_stride)
{
	std::vector<std::complex<float>> swap_buffer;
	swap_buffer.resize(width);

	auto cursor1 = block1;
	auto cursor2 = block2;
	LOG_TRACE(L"<Fft2D> SwapBlock::Before memcpy().", L"BasicRecon");
	for (unsigned int row = 0; row < height; ++row)
	{
		memcpy(swap_buffer.data(), cursor1, width * sizeof(std::complex<float>));
		memcpy(cursor1, cursor2, width * sizeof(std::complex<float>));
		memcpy(cursor2, swap_buffer.data(), width * sizeof(std::complex<float>));

		cursor1 += line_stride;
		cursor2 += line_stride;
	}
	LOG_TRACE(L"<Fft2D> SwapBlock::After memcpy().", L"BasicRecon");
}



// ------����NFFT��r��ֵ--------
void FFT1::Set_NFFT_r(unsigned n) {
	std::bitset<32>bsn(n);
	r = 0;
	while ((n >> r) > 0) {
		r++;
	}

	if ((bsn.count() == 1) && (r > 0)) {
		r--;
	}
	std::bitset<32>bsNFFT(0);
	bsNFFT.set(r);
	NFFT = (unsigned)bsNFFT.to_ulong();
};

// ---- ���ص�����
unsigned FFT1::RevBit(unsigned num) {
	std::bitset<32>bs(num);
	std::bitset<32>bsR;
	// ---------����
	for (unsigned i = 0; i < r; i++) {
		bsR.set(i, bs[r - i - 1]);
	}
	return(unsigned)bsR.to_ulong();
};

// ------------------------------------------------------------------------------
// ���㹫ʽ�е�����AL(ALindex)=AL-1(AL_1index0)+AL-1(AL_1index1)W(Windex)
// ------------------------------------------------------------------------------
void FFT1::GetIndex(unsigned L, unsigned ALindex, unsigned &AL_1index0,
	unsigned &AL_1index1, unsigned &Windex) {
	// ALindexΪAL()������
	// AL_1index0 = 0; // AL-1()�ĵ�һ������
	// AL_1index1 = 0; // AL-1()�ĵڶ�������
	// Windex = 0; // ��λ���е�����
	std::bitset<32>bs(ALindex);
	std::bitset<32>bs1(ALindex);
	bs1.set(r - L, 0);
	AL_1index0 = bs1.to_ulong();
	// -----------------------------------
	std::bitset<32>bs2(ALindex);
	bs2.set(r - L, 1);
	AL_1index1 = bs2.to_ulong();
	// -----------------------------------
	std::bitset<32>bs3; // ��Lλ
	for (unsigned i = 0; i < L; i++) {
		bs3.set(r - L + i, bs[r - i - 1]);
	}
	Windex = bs3.to_ulong();
}

// ------------------------------------
void FFT1::doFFT1() { // һά����Fourier�任
	unsigned AL_1index0 = 0; // AL-1()�ĵ�һ������
	unsigned AL_1index1 = 0; // AL-1()�ĵڶ�������
	unsigned Windex = 0; // ��λ���е�����
	double alpha; // ��λ��
				  // ----------------------------
	std::vector<std::complex<float> >X = TS; // X�м���ʱ����

	for (unsigned L = 1; L <= r; L++) { // ���μ������
		for (unsigned ALindex = 0; ALindex < NFFT; ALindex++) {
			// ALindexΪAL()������
			GetIndex(L, ALindex, AL_1index0, AL_1index1, Windex);
			alpha = -2.0 * sign * pai * Windex / (1.0 * NFFT); // ��ת���ӵ���λ��
			std::complex<float>W(cos(alpha), sin(alpha)); // ��ת����
			FS[ALindex] = X[AL_1index0] + X[AL_1index1] * W;
		}
		X = FS; // ��������
	}
	//
	if (sign > 0) { // Ϊ���任
		for (unsigned i = 0; i < NFFT; i++) {
			FS[i] = X[RevBit(i)]; // �����������Ƶ���
		}
	}
	else { // Ϊ��任
		std::complex<float>WN(NFFT, 0.0); // ���ݵĸ���
		for (unsigned i = 0; i < NFFT; i++) {
			FS[i] = X[RevBit(i)] / WN; // �����������Ƶ���
		}
	}
}

// ----------------------------------------
VVCD FFT2::TranspositionVVCD(VVCD dv) { // ������ת��
	unsigned dvRow = dv.size();
	unsigned maxcol = 0;
	unsigned mincol = 99999;
	VVCD temp;
	if (dv.size() > 0) {
		// ------------------------------
		for (unsigned i = 0; i < dvRow; i++) {
			if (maxcol < dv[i].size()) {
				maxcol = dv[i].size();
			}
			if (mincol > dv[i].size()) {
				mincol = dv[i].size();
			}
		}
		// ------------------------------
		if (mincol == maxcol && mincol > 0) {
			temp.resize(mincol);
			for (unsigned i = 0; i < mincol; i++) {
				temp[i].resize(dvRow);
			}
			for (unsigned i = 0; i < dvRow; i++) {
				for (unsigned j = 0; j < mincol; j++) {
					temp[j][i] = dv[i][j];
				}
			}
		}
	}
	return temp;
}

