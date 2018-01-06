#include "stdafx.h"
#include "Radiomics.h"
#include "Implement\PythonUserImpl.h"
#include "Implement\LogUserImpl.h"
#include "Client\DataHelper.h"

Yap::Radiomics::Radiomics() : 
	_ref_data{ nullptr },
	_python( nullptr ),
	ProcessorImpl(L"Radiomics")
{
	LOG_TRACE(L"Radiomics constructor called.", L"BasicRecon");
	_python = PythonUserImpl::GetInstance().GetPython();

	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeAll);
	AddInput(L"Reference", YAP_ANY_DIMENSION, DataTypeAll);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeAll);

	AddProperty<const wchar_t * const>(L"ScriptPath", L"", L"包含Python脚本文件的文件夹。");
	AddProperty<const wchar_t * const>(L"MethodName", L"", L"脚本文件中需要使用的方法名");
	AddProperty<int>(L"ReturnDataType", DataTypeDouble, L"从Radiomics脚本返回的数据类型");
}

Yap::Radiomics::Radiomics(const Radiomics& rhs) :
	_python(rhs._python), _ref_data(rhs._ref_data), ProcessorImpl(rhs)
{
	LOG_TRACE(L"Radiomics copy constructor.", L"BasicRecon");
}

Yap::Radiomics::~Radiomics()
{
	LOG_TRACE(L"Radiomics destructor called.", L"BasicRecon");
}

bool Yap::Radiomics::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) == L"Reference")
	{
		_ref_data = data;
		return true;
	}
	else if (std::wstring(port) == L"Input" && _ref_data == nullptr) 
	{
		return false;
	}

	auto script = GetProperty<const wchar_t * const>(L"ScriptPath");
	auto method = GetProperty<const wchar_t * const>(L"MethodName");
	auto data_type = data->GetDataType();

	DataHelper help_data(data);
	DataHelper help_ref_data(_ref_data);
	auto input_dims = help_data.GetActualDimensionCount();
	if (help_data.GetActualDimensionCount() != help_ref_data.GetActualDimensionCount() ||
		help_data.GetActualDimensionCount() > 4) // now the largest dimensions is 4.
		return false;

	size_t input_size[4] = { 1 };
	size_t output_dims = 0;
	size_t output_size[4] = { 0 };

	switch (input_dims)
	{
	case 4:
		input_size[3] = static_cast<size_t>(help_data.GetDim4());
	case 3:
		input_size[2] = static_cast<size_t>(help_data.GetSliceCount());
	case 2:
		input_size[1] = static_cast<size_t>(help_data.GetHeight());
	case 1:
		input_size[0] = static_cast<size_t>(help_data.GetWidth());
		break;
	default:
		return false;
	}

	void* out_data = Run(data_type, script, method, input_dims, data, output_dims, input_size, output_size);

	Dimensions dimensions;
	dimensions = CreateDimension(output_dims, output_size);

	auto output_type = GetProperty<int>(L"ReturnDataType");

	return FeedOut(out_data, dimensions, output_type);
}

Yap::Dimensions Yap::Radiomics::CreateDimension(size_t output_dims, size_t * output_size)
{
	Dimensions dimensions;
	switch (output_dims)
	{
	case 4:
		dimensions(DimensionReadout, 0U, (unsigned int)output_size[0])
			(DimensionPhaseEncoding, 0U, (unsigned int)output_size[1])
			(DimensionSlice, 0U, (unsigned int)output_size[2])
			(Dimension4, 0U, (unsigned int)output_size[3]);
		break;
	case 3:
		dimensions(DimensionReadout, 0U, (unsigned int)output_size[0])
			(DimensionPhaseEncoding, 0U, (unsigned int)output_size[1])
			(DimensionSlice, 0U, (unsigned int)output_size[2]);
		break;
	case 2:
		dimensions(DimensionReadout, 0U, (unsigned int)output_size[0])
			(DimensionPhaseEncoding, 0U, (unsigned int)output_size[1]);
		break;
	case 1:
		dimensions(DimensionReadout, 0U, (unsigned int)output_size[0]);
		break;
	default:
		return (Dimensions)nullptr;
	}
	return dimensions;
}

bool Yap::Radiomics::FeedOut(void*& out_data, Dimensions dimensions, int data_type)
{
	switch (data_type)
	{
	case DataTypeInt:
		Feed(L"Output", CreateData<int>(nullptr, reinterpret_cast<int*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeUnsignedInt:
		Feed(L"Output", CreateData<unsigned int>(nullptr, reinterpret_cast<unsigned int*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeShort:
		Feed(L"Output", CreateData<short>(nullptr, reinterpret_cast<short*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeUnsignedShort:
		Feed(L"Output", CreateData<unsigned short>(nullptr, reinterpret_cast<unsigned short*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeChar:
		Feed(L"Output", CreateData<char>(nullptr, reinterpret_cast<char*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeUnsignedChar:
		Feed(L"Output", CreateData<unsigned char>(nullptr, reinterpret_cast<unsigned char*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeFloat:
		Feed(L"Output", CreateData<float>(nullptr, reinterpret_cast<float*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeDouble:
		Feed(L"Output", CreateData<double>(nullptr, reinterpret_cast<double*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeComplexFloat:
		Feed(L"Output", CreateData<std::complex<float>>(nullptr, reinterpret_cast<std::complex<float>*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeComplexDouble:
		Feed(L"Output", CreateData<std::complex<double>>(nullptr, reinterpret_cast<std::complex<double>*>(out_data), dimensions, nullptr, true).get());
		break;
	case DataTypeBool:
		Feed(L"Output", CreateData<bool>(nullptr, reinterpret_cast<bool*>(out_data), dimensions, nullptr, true).get());
		break;
	default:
		return false;
	}
	return true;
}

void* Yap::Radiomics::Run(int data_type, const wchar_t* script, const wchar_t* method, 
	int input_dims, IData * data, size_t output_dims, size_t * input_size, size_t * output_size)
{
	void* out_data = 0;
	switch (data_type)
	{
	case DataTypeChar:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<char>(_ref_data)), 
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<char>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeUnsignedChar:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<unsigned char>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<unsigned short>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeShort:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<short>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<short>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeUnsignedShort:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<unsigned short>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<unsigned short>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeFloat:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<float>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray <float>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeDouble:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<double>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<double>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeInt:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<int>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<int>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeUnsignedInt:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<unsigned int>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<unsigned int>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeComplexFloat:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<std::complex<float>>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<std::complex<float>>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeComplexDouble:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<std::complex<float>>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<std::complex<double>>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeBool:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<bool>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<bool>(data)),
			output_dims, input_size, output_size, false);
		break;
	case DataTypeLongLong:
		_python->SetRefData(reinterpret_cast<void*>(GetDataArray<long long>(_ref_data)),
			data_type, input_dims, input_size);
		out_data = _python->Process(script, method, data_type, input_dims,
			reinterpret_cast<void*>(GetDataArray<long long>(data)),
			output_dims, input_size, output_size, false);
	default:
		break;
	}
	return out_data;
}
