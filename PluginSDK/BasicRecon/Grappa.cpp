#include "Grappa.h"
#include <string>

#include "Interface\SmartPtr.h"
#include "Yap/DataHelper.h"
#include "Interface\DataImp.h"

#include <vector>

using namespace std;
using namespace arma;
using namespace Yap;

CGrappa::CGrappa() :
	CProcessorImp(L"Grappa")
{
	AddProperty(PropertyInt, L"Rate", L"The acceleration factor.");
	SetInt(L"Rate", 2);
	AddProperty(PropertyInt, L"AcsCount", L"The auto-calibration signal.");
	SetInt(L"AcsCount", 16);
	AddProperty(PropertyInt, L"Block", L"The number of blocks.");
	SetInt(L"Block", 4);

	AddInputPort(L"Input", 3, DataTypeComplexDouble | DataTypeComplexFloat);
	AddOutputPort(L"Output", 3, DataTypeComplexDouble | DataTypeComplexFloat);
}


CGrappa::~CGrappa()
{
}

bool CGrappa::Input(const wchar_t * port, IData * data)
{
	if (wstring(port) != L"Input")
		return false;

	auto R = GetInt(L"Rate");
	auto Acs = GetInt(L"AcsCount");
	auto Block = GetInt(L"Block");

	CDataHelper input_data(data);  //输入数据为欠采添零的K空间数据
	if (input_data.GetDataType() != DataTypeComplexDouble && input_data.GetDataType() != DataTypeComplexFloat)
		return false;
	if (input_data.GetActualDimensionCount() != 3)
		return false;

	Recon(reinterpret_cast<complex<double>*>(input_data.GetData()), R, Acs, Block, 
		input_data.GetWidth(), input_data.GetHeight(), input_data.GetCoilCount());

	Feed(L"Output", data);

	return true;
}

std::vector<std::complex<double>> CGrappa::GetAcsData(std::complex<double> * data, 
	size_t r, size_t acs, size_t width, size_t height, size_t num_coil)
{
	unsigned int first = static_cast<unsigned int> (((height - acs) / (2 * r)) * r + 1);
	vector<complex<double>> acs_data(acs * width * num_coil, 0);	
	for (unsigned int coil_index = 0; coil_index < num_coil; ++num_coil)
	{
			for (unsigned int k = 0; k < acs; ++k)
			{
				auto acs_position = width * height * coil_index + (first + k) * width;
				memcpy(acs_data.data()  + width * acs * coil_index +
					k * width, data + acs_position, width * sizeof(complex<double>));
			}
	}
	return acs_data;
}


bool CGrappa::Recon(std::complex<double> * subsampled_data, 
	size_t r, size_t acs, size_t Block, size_t width, size_t height, size_t Num_coil)
{
	unsigned int block = static_cast<unsigned int> (Block);
	unsigned int num_coil = static_cast<unsigned int> (Num_coil);
	vector<complex<double>> acs_data = GetAcsData(subsampled_data, r, acs, width, height, Num_coil);
		cx_mat coef = FitCoef(subsampled_data, r, acs, Block, width, height, Num_coil);
		cx_rowvec Temp(1, block * num_coil * 3);
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
								Temp[coil_index + block_l * num_coil + shift * block * num_coil] = *(subsampled_data - 1 + shift);
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
								Temp[coil_index + (block_r + 2) * num_coil + shift * num_coil] = *(subsampled_data - 1 + shift);
							}
						}
					}
					cx_rowvec Recon_Data = Temp * coef;
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


complex<double> * CGrappa::MakeFidelity(complex<double> * recon_data, vector<complex<double>> acs_data, 
	size_t r, size_t acs, size_t width, size_t height, size_t num_coil)
{
	unsigned int first = static_cast<unsigned int> (((height - acs) / (2 * r)) * r + 1);
	for (unsigned int coil_index = 0; coil_index < num_coil; ++coil_index)
	{
			for (unsigned int k = 0; k < acs; ++k)
			{
				auto acs_position = width * height * coil_index + (first + k) * width;
					memcpy(recon_data + acs_position, acs_data.data() + 
						width * acs * coil_index + k * width, width * sizeof(complex<double>));
			}
	}
	return recon_data;
}



arma::cx_mat CGrappa::FitCoef(complex<double> * subsampled_data, 
	size_t R, size_t acs, size_t Block, size_t Width, size_t height, size_t Num_coil)
{
	unsigned int width = static_cast<unsigned int> (Width);
	unsigned int num_coil = static_cast<unsigned int> (Num_coil);
	unsigned int r = static_cast<unsigned int> (R);
	unsigned int block = static_cast<unsigned int> (Block);
	unsigned int first = static_cast<unsigned int> (((height - acs) / (2 * R)) * R + 1);
	unsigned int fit_num = static_cast<unsigned int> (acs / R);
	cx_mat temp1((width - 2) * fit_num, num_coil * (r - 1));
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

	cx_mat temp2((width - 2) * fit_num, block * num_coil * 3);
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
	cx_mat coef = pinv(temp2) * temp1;
	return coef;
}

Yap::IProcessor * Yap::CGrappa::Clone()
{
	try
	{
		return new CGrappa();
	}
	catch (std::bad_alloc&)
	{
		return nullptr;
	}
}
