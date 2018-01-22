#include "stdafx.h"
#include "Radiomics.h"
#include "Implement\LogUserImpl.h"
#include "Client\DataHelper.h"
#include "Implement\PythonUserImpl.h"

Yap::Radiomics::Radiomics() : 
	_ref_data( YapShared<ISharedObject>(nullptr) ),
	ProcessorImpl( L"Radiomics" )
{
	LOG_TRACE(L"Radiomics constructor called.", L"PythonRecon");

	AddInput(L"Input", YAP_ANY_DIMENSION, DataTypeAll);
	AddInput(L"Reference", YAP_ANY_DIMENSION, DataTypeAll);
	AddOutput(L"Output", YAP_ANY_DIMENSION, DataTypeDouble | DataTypeFloat);

	AddProperty<const wchar_t * const>(L"ScriptPath", L"", L"包含Python脚本文件的文件夹。");
	AddProperty<const wchar_t * const>(L"MethodName", L"", L"脚本文件中需要使用的方法名");
	AddProperty<int>(L"ReturnDataType", DataTypeDouble, L"从Radiomics脚本返回的数据类型");
}

Yap::Radiomics::Radiomics(const Radiomics& rhs) :
	_ref_data(rhs._ref_data), ProcessorImpl(rhs)
{
	LOG_TRACE(L"Radiomics copy constructor.", L"PythonRecon");
}

Yap::Radiomics::~Radiomics()
{
	PYTHON_DELETE_REF_DATA();
	LOG_TRACE(L"Radiomics destructor called.", L"PythonRecon");
}

bool Yap::Radiomics::Input(const wchar_t * port, IData * data)
{
	if (std::wstring(port) == L"Reference")
	{
		if (data == nullptr)
			return false;
		return SetRefData(data);
	}
	else if (std::wstring(port) == L"Input") 
	{
		if (_ref_data.get() == nullptr)
			return false;
	}
	else
	{
		return false;
	}

	int output_type;
	Dimensions dimensions;
	size_t output_dims;
	size_t output_size[4] = { 0,0,0,0 };

	void* out_data = PythonRunScript(data, output_type, dimensions, output_dims, output_size);
	if (out_data == nullptr)
	{
		return false;
	}
	CreateDimension(dimensions, output_dims, output_size);

	//if (dim2.type != DimensionInvalid)
	//{
	//	// auto fileIter_is_finished = GetProperty<bool>(L"FilesIteratorFinished");
	//	bool fileIter_is_finished = false;
	//	if (fileIter_is_finished)
	//	{
	//		_ref_data = YapShared<ISharedObject>(nullptr);
	//		PYTHON_DELETE_REF_DATA();
	//	}
	//}

	return FeedOut(out_data, dimensions, output_type, GetTotalSize(output_dims, output_size));
}

void* Yap::Radiomics::PythonRunScript(IData * data, OUT int & output_type, Dimensions& dimensions, 
	OUT size_t & output_dims, size_t output_size[])
{
	auto script = GetProperty<const wchar_t * const>(L"ScriptPath");
	auto method = GetProperty<const wchar_t * const>(L"MethodName");
	auto data_type = data->GetDataType();

	assert(wcslen(script) != 0 && wcslen(method) != 0);
	DataHelper help_data(data);
	DataHelper help_ref_data(_ref_data.get());
	unsigned int help_dimensions = 0;
	/* add assist_dimensions */
	auto dim1 = help_data.GetDimension(DimensionUser1);
	if (dim1.type != DimensionInvalid)
	{
		++help_dimensions;
		dimensions(dim1.type, dim1.start_index, dim1.length);
	}
	auto dim2 = help_data.GetDimension(DimensionUser2);
	if (dim2.type != DimensionInvalid)
	{
		++help_dimensions;
		dimensions(dim2.type, dim2.start_index, dim2.length);
	}

	auto input_dims = help_data.GetActualDimensionCount() - help_dimensions;
	if (input_dims != help_ref_data.GetActualDimensionCount() - help_dimensions
		|| input_dims > 4) // now the largest dimensions is 4.
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

	/* how to handle out_data to let this processor hold the data instead of Python? */
	
	void * out_data = nullptr;
	switch (data_type)
	{
	case DataTypeInt:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<int>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeUnsignedInt:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<unsigned int>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeShort:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<short>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeUnsignedShort:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<unsigned short>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeChar:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<char>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeUnsignedChar:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<unsigned char>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeComplexFloat:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<std::complex<float>>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeComplexDouble:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<std::complex<double>>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeBool:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<bool>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeFloat:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<float>(data), output_dims, input_size, output_size, true);
		break;
	case DataTypeDouble:
		out_data = PYTHON_PROCESS(script, method, data_type, output_type, input_dims,
			GetDataArray<double>(data), output_dims, input_size, output_size, true);
		break;
	default:
		assert(0 && L"Error data type!");
	}

	int dim_cout = 0;
	for (size_t i = 0; i < 4; ++i)
		dim_cout += (output_size[i] == 0 ? 0 : 1);

	if (out_data == nullptr || dim_cout != output_dims)
	{
		LOG_ERROR(L"Processor::Radiomics do not return any data or wrong dimensions.", L"PythonRecon");
		return nullptr;
	}
	return out_data;
}

size_t Yap::Radiomics::GetTotalSize(size_t dimension_count, size_t size[])
{
	size_t total_size = 1;
	for (size_t i = 0; i < dimension_count; ++i)
	{
		total_size *= size[i];
	}

	return total_size;
}

bool Yap::Radiomics::SetRefData(IData * data)
{
	DataHelper h(data);
	_ref_data = YapShared(data);
	unsigned int help_dimensions = 0;
	auto dim1 = h.GetDimension(DimensionUser1);
	if (dim1.type != DimensionInvalid)
		++help_dimensions;
	auto dim2 = h.GetDimension(DimensionUser2);
	if (dim2.type != DimensionInvalid)
		++help_dimensions;
	auto input_dims = h.GetActualDimensionCount() - help_dimensions;
	if (input_dims > 4)
		return false;
	size_t input_size[4] = { 1,1,1,1 };
	switch (input_dims)
	{
	case 4:
		input_size[3] = static_cast<size_t>(h.GetDimension(Dimension4).length);
	case 3:
		input_size[2] = static_cast<size_t>(h.GetDimension(DimensionSlice).length);
	case 2:
		input_size[1] = static_cast<size_t>(h.GetDimension(DimensionPhaseEncoding).length);
	case 1:
		input_size[0] = static_cast<size_t>(h.GetDimension(DimensionReadout).length);
		break;
	default:
		return false;
	}
	switch (h.GetDataType())
	{
	case DataTypeInt:
		PYTHON_SET_REF_DATA(GetDataArray<int>(data), h.GetDataType(), input_dims, input_size);
		break;
	case DataTypeUnsignedInt:
		PYTHON_SET_REF_DATA(GetDataArray<unsigned int>(data), h.GetDataType(), input_dims, input_size);
		break;
	case DataTypeShort:
		PYTHON_SET_REF_DATA(GetDataArray<short>(data), h.GetDataType(), input_dims, input_size);
		break;
	case DataTypeUnsignedShort:
		PYTHON_SET_REF_DATA(GetDataArray<unsigned short>(data), h.GetDataType(), input_dims, input_size);
		break;
	case DataTypeChar:
		PYTHON_SET_REF_DATA(GetDataArray<char>(data), h.GetDataType(), input_dims, input_size); 
		break;
	case DataTypeUnsignedChar:
		PYTHON_SET_REF_DATA(GetDataArray<unsigned char>(data), h.GetDataType(), input_dims, input_size);
		break;
	case DataTypeComplexFloat:
		PYTHON_SET_REF_DATA(GetDataArray<std::complex<float>>(data), h.GetDataType(), input_dims, input_size);
		break;
	case DataTypeComplexDouble:
		PYTHON_SET_REF_DATA(GetDataArray<std::complex<double>>(data), h.GetDataType(), input_dims, input_size);
		break;
	case DataTypeBool:
		PYTHON_SET_REF_DATA(GetDataArray<bool>(data), h.GetDataType(), input_dims, input_size);
		break;
	case DataTypeFloat:
		PYTHON_SET_REF_DATA(GetDataArray<float>(data), h.GetDataType(), input_dims, input_size);
		break;
	case DataTypeDouble:
		PYTHON_SET_REF_DATA(GetDataArray<double>(data), h.GetDataType(), input_dims, input_size);
		break;
	default:
		assert(0 && L"Error data type!");
		return false;
	}
	return true;
}

void Yap::Radiomics::CreateDimension(Dimensions & dimensions, size_t output_dims, size_t * output_size)
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

bool Yap::Radiomics::FeedOut(void* out_data, Dimensions dimensions, int data_type, size_t size)
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
			auto data = CreateData<float>(nullptr, reinterpret_cast<float*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		case DataTypeDouble:
		{
			auto data = CreateData<double>(nullptr, reinterpret_cast<double*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		default:
			Feed(L"Output", nullptr);
			return false;
		/*case DataTypeInt:
		{
			auto data = CreateData<int>(nullptr, reinterpret_cast<int*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		case DataTypeUnsignedInt:
		{
			auto data = CreateData<unsigned int>(nullptr, reinterpret_cast<unsigned int*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		case DataTypeShort:
		{
			auto data = CreateData<short>(nullptr, reinterpret_cast<short*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		case DataTypeUnsignedShort:
		{
			auto data = CreateData<unsigned short>(nullptr, reinterpret_cast<unsigned short*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		case DataTypeChar:
		{
			auto data = CreateData<char>(nullptr, reinterpret_cast<char*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		case DataTypeUnsignedChar:
		{
			auto data = CreateData<unsigned char>(nullptr, reinterpret_cast<unsigned char*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		case DataTypeComplexFloat:
		{
			auto data = CreateData<std::complex<float>>(nullptr, reinterpret_cast<std::complex<float>*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		case DataTypeComplexDouble:
		{
			auto data = CreateData<std::complex<double>>(nullptr, reinterpret_cast<std::complex<double>*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}
		case DataTypeBool:
		{
			auto data = CreateData<bool>(nullptr, reinterpret_cast<bool*>(out_my_data), dimensions);
			Feed(L"Output", data.get());
			break;
		}*/
	}
	return true;
}
