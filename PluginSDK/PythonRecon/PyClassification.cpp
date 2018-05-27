#include "PyClassification.h"
#include "Implement\LogUserImpl.h"
#include "Client\DataHelper.h"
#include "Implement\PythonUserImpl.h"

using namespace Yap;

Yap::PyClassification::PyClassification():
	ProcessorImpl(L"PyClassification")
{
	AddInput(L"Input", 2U, DataTypeFloat);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeAll);

	AddProperty<std::wstring>(L"ScriptPath", L"", L"Python classification script path");
	AddProperty<std::wstring>(L"MethodName", L"", L"classification method in ScriptPath");
	AddProperty<int>(L"ReturnDataType", 16, L"return data type from python");
}

Yap::PyClassification::~PyClassification()
{

}

bool Yap::PyClassification::Input(const wchar_t * name, IData * data)
{
	assert(std::wstring(name) != L"Input");
	assert(data != nullptr);
	
	int output_type = DataTypeUnknown;
	Dimensions dimensions;
	size_t output_dims = 0;
	size_t output_size[4] = { 0,0,0,0 };

	void* out_data = PythonRunScript(data, output_type, dimensions, output_dims, output_size);
	if (out_data == nullptr)
	{
		LOG_ERROR(L"for some reason ExtractFeatures does not get return value from python", L"ExtractFeatures--PythonRecon");
		return false;
	}
	CreateDimension(dimensions, output_dims, output_size);

	return FeedOut(data, out_data, dimensions, output_type, GetTotalSize(output_dims, output_size));
	
}

void* Yap::PyClassification::PythonRunScript(IData * data, OUT int & output_type, Dimensions& dimensions,
	OUT size_t & output_dims, size_t output_size[])
{
	auto script_path = GetProperty<std::wstring>(L"ScriptPath");
	auto script = script_path.c_str();
	auto method_name = GetProperty<std::wstring>(L"MethodName");
	auto method = method_name.c_str();
	auto data_type = data->GetDataType();

	assert(wcslen(script) != 0 && wcslen(method) != 0);
	DataHelper help_data(data);

	auto input_dims = help_data.GetActualDimensionCount();
	if (input_dims > 4) 
		return nullptr;

	size_t input_size[4] = { 1,1,1,1 };
	switch (input_dims)
	{
	case 4:
		input_size[3] = static_cast<size_t>(help_data.GetDimension(Dimension4).length);
	case 3:
		input_size[2] = static_cast<size_t>(help_data.GetDimension(DimensionSlice).length);
	case 2:
		input_size[1] = static_cast<size_t>(help_data.GetDimension(DimensionPhaseEncoding).length);
	case 1:
		input_size[0] = static_cast<size_t>(help_data.GetDimension(DimensionReadout).length);
		break;
	default:
		return nullptr;
	}
	output_type = GetProperty<int>(L"ReturnDataType");

	void * out_data = nullptr;
	switch (data_type)
	{
	case DataTypeInt:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<int>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeUnsignedInt:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<unsigned int>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeShort:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<short>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeUnsignedShort:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<unsigned short>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeChar:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<char>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeUnsignedChar:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<unsigned char>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeComplexFloat:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<std::complex<float>>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeComplexDouble:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<std::complex<double>>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeBool:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<bool>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeFloat:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<float>(data), output_dims, input_size, output_size, false);
		break;
	case DataTypeDouble:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<double>(data), output_dims, input_size, output_size, false);
		break;
	default:
		assert(0 && L"Error data type!");
	}

	int dim_cout = 0;
	for (size_t i = 0; i < 4; ++i)
		dim_cout += (output_size[i] == 0 ? 0 : 1);

	if (out_data == nullptr || dim_cout != output_dims)
	{
		LOG_ERROR(L"Processor::ExtractFeatures do not return any data or wrong dimensions.", L"PythonRecon");
		return nullptr;
	}
	return out_data;
}

void Yap::PyClassification::CreateDimension(Dimensions & dimensions, size_t output_dims, size_t * output_size)
{
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
		assert(0 && L"script return data Dimension size error!");
		break;
	}
}

bool Yap::PyClassification::FeedOut(IData * inut_data, void* out_data, Dimensions dimensions, int data_type, size_t size)
{
	void * out_my_data;
	try
	{
		if (data_type == DataTypeDouble)
		{
			size *= sizeof(double);
			out_my_data = new  double[size];
		}
		if (data_type == DataTypeFloat)
		{
			size *= sizeof(float);
			out_my_data = new  float[size];
		}
		else
		{
			return false;
		}
		memcpy(out_my_data, out_data, size);
		// delete[] out_data;
	}
	catch (const std::bad_alloc&)
	{
		return false;
	}

	switch (data_type)
	{
	case DataTypeFloat:
	{
		auto data = CreateData<float>(inut_data, reinterpret_cast<float*>(out_my_data), dimensions);
		Feed(L"Output", data.get());
		break;
	}
	case DataTypeDouble:
	{
		auto data = CreateData<double>(inut_data, reinterpret_cast<double*>(out_my_data), dimensions);
		Feed(L"Output", data.get());
		break;
	}
	default:
		LOG_ERROR(L"ReturnDataType set error", L"ExtractFeatures--PythonRecon");
		Feed(L"Output", nullptr);
		return false;
	}
	return true;
}

size_t Yap::PyClassification::GetTotalSize(size_t dimension_count, size_t size[])
{
	size_t total_size = 1;
	for (size_t i = 0; i < dimension_count; ++i)
	{
		total_size *= size[i];
	}

	return total_size;
}