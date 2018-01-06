#include "stdafx.h"
#include "cuFft2D.h"
#include "Client/DataHelper.h"
#include "Implement/DataObject.h"


using namespace std;
using namespace Yap;

extern "C" void pre_cuFft2D(std::complex<float> * h_kspace, std::complex<float> * h_image, bool fft_forward, unsigned int width, unsigned int height);

cuFft2D::cuFft2D():
	ProcessorImpl(L"cuFft2D")
{
	AddProperty(L"fft_forward", true, L"The direction of cuFFT2D.");
	
	AddInput(L"Input", 2, DataTypeComplexFloat);
	AddOutput(L"Output", 2, DataTypeComplexFloat);
}


cuFft2D::~cuFft2D()
{
}

//IProcessor * Yap::cuFft2D::Clone()
//{
//	return new (nothrow) cuFft2D(*this);
//}

bool Yap::cuFft2D::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	DataHelper input_data(data);
	if (input_data.GetDataType() != DataTypeComplexFloat)
		return false;

	if (input_data.GetActualDimensionCount() != 2)
		return false;

	auto width = input_data.GetWidth();
	auto height = input_data.GetHeight();
	auto data_array = GetDataArray<complex<float>>(data);
	//auto out_data = YapShared(new ComplexFloatData(data->GetDimensions()));//xhb->
	auto output = CreateData<complex<double>>(data);
	pre_cuFft2D(data_array, GetDataArray<complex<float>>(output.get()), GetProperty<bool>(L"fft_forward"), width, height);

	Feed(L"Output", output.get());
	return true;
}


