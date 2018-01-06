#include "stdafx.h"
#include "NLM.h"
#include "Client/DataHelper.h"
#include "Implement/DataObject.h"
#include "Implement/LogUserImpl.h"

using namespace std;
using namespace Yap;
using namespace arma;

NLM::NLM(void):
	ProcessorImpl(L"NLM")
{
	LOG_TRACE(L"NLM constructor called.", L"BasicRecon");
	AddInput(L"Input", 2, DataTypeFloat);
	AddOutput(L"Output", 2, DataTypeFloat);
}

NLM::NLM(const NLM& rhs)
	:ProcessorImpl(rhs)
{
	LOG_TRACE(L"NLM copy constructor called.", L"BasicRecon");
}

NLM::~NLM()
{
	LOG_TRACE(L"NLM destructor called.", L"BasicRecon");
}

bool Yap::NLM::Input(const wchar_t * name, IData * data)
{
	assert((data != nullptr) && Yap::GetDataArray<float>(data) != nullptr);
	assert(Inputs()->Find(name) != nullptr);

	if (data->GetDataType() != DataTypeFloat)
		return false;

//	float sigma = GetFloat(L"Sigma");

	DataHelper input_data(data);
	unsigned int width = input_data.GetWidth();
	unsigned int height = input_data.GetHeight();

	float * input_img = GetDataArray<float>(data);
	auto output_img = CreateData<float>(data);

	float sigma = GetSigma(input_img, width, height);

	unsigned int sw_r, pl_r;
	float h;

	if (sigma > 0.0f && sigma <= 15.0f) 
	{
		pl_r = 1;
		sw_r = 10;
		h = 0.4f;
	}
	else if (sigma > 15.0f && sigma <= 30.0f) 
	{
		pl_r = 2;
		sw_r = 10;
		h = 0.4f;
	}
	else if (sigma > 30.0f && sigma <= 45.0f) 
	{
		pl_r = 3;
		sw_r = 17;
		h = 0.35f;
	}
	else if (sigma > 45.0f && sigma <= 75.0f) 
	{
	    pl_r = 4;
		sw_r = 17;
		h = 0.35f;
	}
	else if (sigma <= 100.0f) 
	{
		pl_r = 5;
		sw_r = 17;
		h = 0.30f;
	}
	else
	{
		return false;
	}

	nlmeans(input_img, GetDataArray<float>(output_img.get()), pl_r, sw_r, sigma, h, width, height);

	Feed(L"Output", output_img.get());

	return true;
}

void Yap::NLM::nlmeans(float * input_img, float * output_img, unsigned int pl_r, 
	unsigned int sw_r, float sigma, float h, unsigned int width, unsigned int height)
{
	unsigned int sw = 2 * sw_r + 1;
	unsigned int pl = 2 * pl_r + 1;
	float h2 = h * h;
	vector<float> input_data(height * width);
	memcpy(input_data.data(), input_img, width * height * sizeof(float));
	vector<float> output_data(height * width);

	fmat kernel = GetGaussianKernel(pl, sigma);

	fmat image_pad(height + 2 * (sw_r + pl_r), width + 2 * (sw_r + pl_r));
	image_pad.zeros();
	
	for (unsigned int row = 0; row < height; ++row)
	{
		for (unsigned int col = 0; col < width; ++col)
		{
			image_pad(row + sw_r + pl_r, col + sw_r + pl_r)
				= input_data[row * width + col];
		}
	}

	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			fmat patch_base = image_pad(span(i + sw_r, i + sw_r + pl - 1), span(j + sw_r,  j + sw_r + pl - 1));
			fmat d(sw, sw);
			d.zeros();
			fmat pixel(sw, sw);
			pixel.zeros();

			for (unsigned int x = 0; x < sw; ++x)
			{
				for (unsigned int y = 0; y < sw; ++y)
				{
					if (x == sw_r + 1 && y == sw_r + 1)
					{
						d(x, y) = d(1, 1);
						pixel(x, y) = image_pad(i + sw_r + pl_r, j + sw_r + pl_r);
						continue;
					}
					fmat patch_shift = image_pad(span(i + x, i + pl + x - 1), span(j + y,  j + pl + y - 1));
					fmat distance = (patch_base - patch_shift) % (patch_base - patch_shift);
					d(x, y) = accu(distance % kernel);
					pixel(x, y) = image_pad(i + pl_r + x, j + pl_r + y);
				}
			}

			d(sw_r + 1, sw_r + 1) = min(vectorise(d));
			fmat weight = exp(-d/h2);
			float W = accu(weight);
			if (W == 0)
			{
				W = 1;
			}
			output_data[i * width + j] = sum(sum(pixel % weight)) / W;
		}
	}
	memcpy(output_img, output_data.data(), width * height * sizeof(float));
}

arma::fmat Yap::NLM::GetGaussianKernel(int pl, float sigma)
{
	const float PI = 4.0f * atan(1.0f);
	fmat kernel(pl, pl);
	kernel.zeros();
	int center = (pl - 1) / 2;
	float sum = 0;
	for (int i = 0; i < pl; ++i)
	{
		for (int j = 0; j < pl; ++j)
		{
			kernel(i, j) = exp(-((i - center)*(i - center) + (j - center)*(j - center)) / (2 * sigma*sigma)) / (2 * PI*sigma*sigma);
			sum += kernel(i, j);
		}
	}
	for (int i = 0; i < pl; ++i)
	{
		for (int j = 0; j < pl; ++j)
		{
			kernel(i, j) /= sum;
		}
	}

	return kernel;
}

float Yap::NLM::GetSigma(float * input_img, unsigned int width, unsigned int height)
{
	vector<float> input_data(width * height);
	fmat input(height, width);
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
