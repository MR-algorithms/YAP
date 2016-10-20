#include "Grappa.h"
#include <string>

#include "Interface/Client/DataHelper.h"
#include "Interface/Implement/DataObject.h"

#include <vector>


using namespace std;
using namespace arma;
using namespace Yap;

Grappa::Grappa(void) :
	ProcessorImpl(L"Grappa")
{
}

Yap::Grappa::Grappa(const Grappa & rhs)
	: ProcessorImpl(rhs)
{
}

Grappa::~Grappa()
{
}


bool Yap::Grappa::OnInit()
{
	AddProperty(PropertyInt, L"Rate", L"The acceleration factor.");
	SetInt(L"Rate", 2);
	AddProperty(PropertyInt, L"AcsCount", L"The auto-calibration signal.");
	SetInt(L"AcsCount", 32);
	AddProperty(PropertyInt, L"Block", L"The number of blocks.");
	SetInt(L"Block", 4);

	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeComplexDouble | DataTypeComplexFloat);

	return true;
}


bool Grappa::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;
		auto R = GetInt(L"Rate");
		auto Acs = GetInt(L"AcsCount");
		auto Block = GetInt(L"Block");

		DataHelper input_data(data);  //输入数据为欠采添零的K空间数据
		if (input_data.GetDataType() != DataTypeComplexDouble && input_data.GetDataType() != DataTypeComplexFloat)
			return false;
 		if (input_data.GetActualDimensionCount() != 3)
 			return false;
		auto width = input_data.GetWidth();
		auto height = input_data.GetHeight();
		Dimension channel_dimension = input_data.GetDimension(DimensionChannel);
		Recon(GetDataArray<complex<float>>(data), R, Acs, Block,
			width, height, channel_dimension.length);

		Feed(L"Output", data);

	return true;
}

std::vector<std::complex<float>> Grappa::GetAcsData(std::complex<float> * data, 
	unsigned int r, unsigned int acs, unsigned int width, unsigned int height, unsigned int num_coil)
{
	unsigned int first = ((height - acs) / (2 * r)) * r + 1;
	vector<complex<float>> acs_data(acs * width * num_coil);
	for (unsigned int coil_index = 0; coil_index < num_coil; ++coil_index)
	{
		auto acs_position = width * height * coil_index + first * width;
		memcpy(acs_data.data() + width * acs * coil_index, data + acs_position, acs * width * sizeof(complex<float>));
	}
	return acs_data;
}


bool Grappa::Recon(std::complex<float> * subsampled_data, 
	unsigned int  r, unsigned int acs, unsigned int block, unsigned int width, unsigned int height, unsigned int num_coil)
{
	vector<complex<float>> acs_data = GetAcsData(subsampled_data, r, acs, width, height, num_coil);
		cx_fmat coef = FitCoef(subsampled_data, r, acs, block, width, height, num_coil);

		cx_frowvec Temp(1, block * num_coil * 3);
		Temp.zeros();
		for (unsigned int n = 0; n < floor(height / r); ++n)
		{
			for (unsigned int readout_index = 1; readout_index < width - 1; ++readout_index)
			{
				for (unsigned int shift = 0; shift < 3; ++shift)
				{
					for (unsigned int block_l = 0; block_l < block / 2; ++block_l)
					{
						for (unsigned int coil_index = 0; coil_index < num_coil; ++coil_index)
						{
							if (n < block_l)
							{
								Temp[coil_index + block_l * num_coil + shift * block * num_coil] = 0;
							}
							else
							{
								auto src_point =width * height * coil_index + 
									width * (1 + n * r - block_l * r - 1) + readout_index;
								Temp[coil_index + block_l * num_coil + shift * block * num_coil] = *(subsampled_data + src_point + shift - 1);
							}
						}
					}
					for (unsigned int block_r = 0; block_r < block / 2; ++block_r)
					{
						for (unsigned int coil_index = 0; coil_index < num_coil; ++coil_index)
						{
							if ((block_r + n + 1) * r + 1 > height)
							{
								Temp[coil_index + (block_r + 2) * num_coil + shift * num_coil] = 0;
							}
							else
							{
								auto src_point =width * height * coil_index + 
									width * (1 + n * r + block_r * r + 1) + readout_index;
								Temp[coil_index + (block_r + 2) * num_coil + shift * num_coil] = *(subsampled_data + src_point - 1 + shift);
							}
						}
					}
					cx_frowvec Recon_Data = Temp * coef;
					for (unsigned int b = 0; b < r - 1; ++b)
					{
						for (unsigned int coil_index = 0; coil_index < num_coil; ++coil_index)
						{
							auto recon_point =width * height * coil_index + 
								width * (r * n + 1 + b) + readout_index;
							*(subsampled_data + recon_point) = Recon_Data[coil_index + b * num_coil];
						}
					}
				}
			}
		}

		MakeFidelity(subsampled_data, acs_data, r, acs, width, height, num_coil);
	return true;
}


complex<float> * Grappa::MakeFidelity(complex<float> * recon_data, vector<complex<float>> acs_data, 
	unsigned int r, unsigned int acs, unsigned int width, unsigned int height, unsigned int num_coil)
{
	unsigned int first = ((height - acs) / (2 * r)) * r + 1;
	for (unsigned int coil_index = 0; coil_index < num_coil; ++coil_index)
	{
				auto acs_position = width * height * coil_index + first * width;
					memcpy(recon_data + acs_position, acs_data.data() + 
						width * acs * coil_index, acs * width * sizeof(complex<float>));
	}
	return recon_data;
}



arma::cx_fmat Grappa::FitCoef(complex<float> * subsampled_data, 
	unsigned int r, unsigned int acs, unsigned int block, unsigned int width, unsigned int height, unsigned int num_coil)
{
	unsigned int first =((height - acs) / (2 * r)) * r + 1;
	unsigned int fit_num = acs / r;
	cx_fmat temp1((width - 2) * fit_num, num_coil * (r - 1));
	temp1.zeros();
	for (unsigned int k = 0; k < fit_num; ++k)
	{
		for (unsigned int b = 0; b < r - 1; ++b)
		{
			for (unsigned int readout_index = 1; readout_index < width - 1; ++readout_index)
			{
				for (unsigned int coil_index = 0; coil_index < num_coil; ++coil_index)
				{
					auto acs_point = width * height * coil_index + width * (first + k * r + b) + readout_index;
					temp1((k * (width - 2) + readout_index - 1), coil_index + b * num_coil) = *(subsampled_data + acs_point);
				}
			}
		}
	}

	cx_fmat temp2((width - 2) * fit_num, block * num_coil * 3);
	temp2.zeros();
	for (unsigned int k = 0; k < fit_num; ++k)
	{
		for (unsigned int readout_index = 1; readout_index < width - 1; ++readout_index)
		{
			for (unsigned int shift = 0; shift < 3; ++shift)
			{
				for (unsigned int block_l = 0; block_l < block / 2; ++block_l)
				{
					for (unsigned int coil_index = 0; coil_index < num_coil; ++coil_index)
					{
						auto src_point =width * height * coil_index + 
							width * (first + k * r - block_l * r - 1) + readout_index;
						temp2((k * (width - 2) + readout_index - 1), coil_index + block_l * num_coil + shift * block * num_coil)=
							*(subsampled_data + src_point - 1 + shift);
					}
				}
				for (unsigned int block_r = 0; block_r < block / 2; ++block_r)
				{
					for (unsigned int coil_index = 0; coil_index < num_coil; ++coil_index)
					{
						auto src_point =width * height * coil_index + 
							width * (first + k * r + block_r * r + 1) + readout_index;
						temp2((k * (width - 2) + readout_index - 1), coil_index + (block_r + 2) * num_coil + shift * block * num_coil) =
							*(subsampled_data + src_point - 1 + shift);
					}
				}
			}
		}
	}
	cx_fmat coef = pinv(temp2, 0, "std") * temp1;
	return coef;
}

Yap::IProcessor * Yap::Grappa::Clone()
{
	return new (nothrow) Grappa(*this);
}


