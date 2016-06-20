#include "stdafx.h"
#include "ZeroFilling.h"
#include <string>
#include "..\Interface\SmartPtr.h"
#include "DataHelper.h"


CZeroFilling::CZeroFilling():
	_output_image(0)
{
	AddInputPort(L"Input", 2, DataTypeUnknown);
	AddOutputPort(L"Output", 2, DataTypeUnknown);
}


CZeroFilling::~CZeroFilling()
{
}

bool CZeroFilling::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) != L"Input")
		return false;

	auto properties = GetProperties();
	if (properties->GetProperty(L"Dimension Readout") == nullptr)
		return false;
	if (properties->GetProperty(L"Dimension PhaseEncoding") == nullptr)
		return false;

	auto output_readout = GetIntProperty(L"Dimension Readout");
	auto output_phase_encoding = GetIntProperty(L"Dimension PhaseEncoding");

	ZeroFilling(data, output_readout, output_phase_encoding);

	Yap::CDimensions output_dimension;
	output_dimension(DimensionReadout, 0, output_readout)
		(DimensionPhaseEncoding, 0, output_phase_encoding);

	auto * output_data = new Yap::CDoubleData(_output_image.data(), output_dimension, nullptr, false);

	Feed(L"Output", output_data);

	return true;
}

wchar_t * CZeroFilling::GetId()
{
	return L"ZeroFilling";
}

bool CZeroFilling::ZeroFilling(IData * input, unsigned int output_readout, unsigned int output_phase_encoding)
{
	CDataHelper input_data(input);
	if (input_data.GetDimensionCount() != 2)
		return false;

	auto input_image = reinterpret_cast<double*>(input_data.GetData());
	auto input_readout = input_data.GetWidth();
	auto input_phase_encoding = input_data.GetHeight();

	_output_image.resize(output_readout * output_phase_encoding);
	for (unsigned int i = 0; i < _output_image.size(); ++i)
	{
		_output_image[i] = 0.0;
	}

	for (unsigned int i = 0; i < input_readout * input_phase_encoding; ++i)
	{
		unsigned int input_x(0);
		unsigned int input_y(0);
		unsigned int output_x(0);
		unsigned int output_y(0);

		if (i % input_readout == 0)
		{
			input_x = i / input_readout;
			input_y = input_readout;
		}
		else
		{
			input_x = (unsigned int)ceil(i / input_readout);
			input_y = i % input_readout;
		}

		output_x = input_x + (unsigned int)floor((output_phase_encoding - input_phase_encoding) / 2);
		output_y = input_y + (unsigned int)floor((output_readout - input_readout) / 2);

		auto output_index = (output_x - 1) * output_readout + output_y;

		_output_image[output_index] = input_image[i];
	}

	return true;
}
