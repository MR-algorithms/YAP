#include "PythonImpl.h"
#include "Interface\IPython.h"
#include "Interface\IData.h"
#include "Utilities\macros.h"

#include <boost\python.hpp>

#include <Windows.h>
#include <map>
#include <vector>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

using namespace std;
using namespace Yap;

namespace bpy = boost::python;

class PythonImpl : public IPython
{
public:
	~PythonImpl();

	static PythonImpl& GetInstance();

	virtual void* Process(const wchar_t* module, const wchar_t* method, int data_type, int out_data_type,
		size_t input_dimensions, void * data, OUT size_t& output_dimensions,
		size_t input_size[], OUT size_t output_size[], bool is_need_ref_data = false) override;

	virtual void SetRefData(void * ref_data, int data_type, size_t dimension_count, size_t size[]) override;

	virtual void DeleteRefData() override;

private:
	PythonImpl();

	size_t GetTotalSize(size_t dimension_count, size_t size[]);

	bpy::list CArray2Pylist(void* data, int data_type, size_t dimension_count, size_t size[]);

	/*convert 1D array to python 2D list*/
	template<typename T>
	bpy::list DoCArray2Pylist(const T*& data, size_t dimension_count, size_t size[]);

	void* Pylist2CArray(const bpy::list& li, int out_data_type, size_t dimension_count, size_t size[]);

	/* use function:[ pylist2Vector ] to convert pylist to vector, then memory allocate to c++ array. */
	template<typename T>
	void DoPylist2CArray(const bpy::list& li, T*& out_data, size_t dim_count, size_t size[]);

	template<typename T>
	void pylist2Array(const bpy::object& iterable, T *& out_data, size_t size);

	template<>
	void pylist2Array(const bpy::object& iterable, bool *& out_data, size_t size);

	std::string ToMbs(const wchar_t * wcs);

	void* _ref_data;
	bpy::list _ref_data_list;
	static std::shared_ptr<PythonImpl> s_instance;

protected:
	
	CRITICAL_SECTION g_cs;

	friend struct MyCriticalSection;
};

shared_ptr<PythonImpl> PythonImpl::s_instance;

struct MyCriticalSection {
	MyCriticalSection()
	{
		EnterCriticalSection(&PythonImpl::s_instance->g_cs);
	};
	~MyCriticalSection()
	{
		LeaveCriticalSection(&PythonImpl::s_instance->g_cs);
	}
};

PythonImpl::PythonImpl()
{
	if (Py_IsInitialized())
		return;

	Py_InitializeEx(0);
	if (!Py_IsInitialized())
		return;

	TODO(L"Is that right to add critical section in constructor?");
	InitializeCriticalSection(&g_cs);
}

size_t PythonImpl::GetTotalSize(size_t dimension_count, size_t size[])
{
	size_t total_size = 1;
	for (size_t i = 0; i < dimension_count; ++i)
	{
		total_size *= size[i];
	}

	return total_size;
}

bpy::list PythonImpl::CArray2Pylist(void* data, int data_type, size_t dimension_count, size_t size[])
{
	vector<size_t> reversed_size(dimension_count);
	for (size_t i = 0; i < dimension_count; ++i)
	{
		reversed_size[i] = size[dimension_count - i - 1];
	}

	switch (data_type)
	{
	case DataTypeInt:
	{
		const int* p = reinterpret_cast<int*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeUnsignedInt:
	{
		const unsigned int* p = reinterpret_cast<unsigned int*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeChar:
	{
		const char* p = reinterpret_cast<char*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeUnsignedChar:
	{
		const unsigned char* p = reinterpret_cast<unsigned char*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeShort:
	{
		const short* p = reinterpret_cast<short*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeUnsignedShort:
	{
		const unsigned short* p = reinterpret_cast<unsigned short*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeFloat:
	{
		const float* p = reinterpret_cast<float*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeDouble:
	{
		const double* p = reinterpret_cast<double*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeComplexFloat:
	{
		const complex<float>* p = reinterpret_cast<complex<float>*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeComplexDouble:
	{
		const complex<double>* p = reinterpret_cast<complex<double>*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeBool:
	{
		const bool* p = reinterpret_cast<bool*>(data);
		return DoCArray2Pylist(p, dimension_count, reversed_size.data());
	}
	case DataTypeUnknown:
	default:
	{
		assert(0 && L"The input type is invalid type!\n");
		bpy::list temp;
		return temp;
	}
	}
}

template<typename T>
bpy::list PythonImpl::DoCArray2Pylist(const T*& data, size_t dimension_count, size_t size[])
{
	bpy::list result;
	if (dimension_count == 1)
	{
		/*std::vector<T> v(size[0]);
		v.assign(data, data + size[0]);

		// bpy::object iter = bpy::iterator<std::vector<T>, std::vector<T>::iterator()>(v);

		bpy::object get_iter = bpy::iterator<std::vector<T> >();
		bpy::object iter = get_iter(v);
		bpy::list li(iter);
		result(iter);
		data += size[0];*/
		for (size_t i = 0; i < size[0]; ++i)
		{
			result.append(*data++);
		}
	}
	else
	{
		for (size_t i = 0; i < size[0]; ++i)
		{
			result.append(DoCArray2Pylist(data, dimension_count - 1, size + 1));
		}
	}
	return result;
}

void* PythonImpl::Pylist2CArray(const bpy::list& li, int out_data_type, size_t dimension_count, size_t size[])
{
	size_t * reversed_size = new size_t[4];
	// vector<size_t> reversed_size(dimension_count);
	for (size_t i = 0; i < dimension_count; ++i)
	{
		reversed_size[i] = size[dimension_count - i - 1];
	}

	switch (out_data_type)
	{
	case DataTypeInt:
	{
		int * out_data = new  int[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeUnsignedInt:
	{
		unsigned int * out_data = new  unsigned int[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeChar:
	{
		char* out_data = new  char[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeUnsignedChar:
	{
		unsigned char* out_data = new  unsigned char[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeShort:
	{
		short* out_data = new  short[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeUnsignedShort:
	{
		unsigned short* out_data = new  unsigned short[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeFloat:
	{
		float* out_data = new  float[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeDouble:
	{
		double*out_data = new  double[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeComplexFloat:
	{
		complex<float>* out_data = new  complex<float>[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeComplexDouble:
	{
		complex<double>* out_data = new  complex<double>[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeBool:
	{
		bool* out_data = new  bool[GetTotalSize(dimension_count, size)];
		DoPylist2CArray(li, out_data, dimension_count, reversed_size);
		return out_data;
	}
	case DataTypeUnknown:
	default:
		return nullptr;
	}
}

template<typename T>
void PythonImpl::DoPylist2CArray(const bpy::list& li, T*& out_data, size_t dim_count, size_t size[])
{
	assert(out_data != nullptr);

	if (dim_count == 1)
	{
		pylist2Array(li, out_data, size[0]);
		/*for (size_t i = 0; i < size[0]; ++i)
		{
			*(out_data++) = bpy::extract<T>(li[i]);
		}*/
	}
	else
	{
		for (size_t i = 0; i < size[0]; ++i)
		{
			auto sub_list = bpy::extract<bpy::list>(li[i]);
			DoPylist2CArray(sub_list, out_data, dim_count - 1, size + 1);
		}

	}
}

template<typename T>
void PythonImpl::pylist2Array(const bpy::object& iterable, T *& out_data, size_t size)
{
	auto tv = std::vector<T>(bpy::stl_input_iterator<T>(iterable),
		bpy::stl_input_iterator<T>());
	T * source_data = tv.data();
	memcpy(reinterpret_cast<void*>(out_data), reinterpret_cast<void*>(source_data), (size_t)(size * sizeof(T)));
	out_data += size;
}

template<>
void PythonImpl::pylist2Array(const bpy::object& iterable, bool *& out_data, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		*(out_data++) = bpy::extract<bool>(iterable[i]);
	}
}

std::string PythonImpl::ToMbs(const wchar_t * wcs)
{
	assert(wcslen(wcs) < 500 && "The string cannot contain more than 500 characters.");

	static char buffer[1024];
	size_t size;

	wcstombs_s(&size, buffer, (size_t)1024, wcs, (size_t)1024);
	return std::string(buffer);
}

PythonImpl::~PythonImpl()
{
	/*if (!Py_IsInitialized())
	{
		DeleteCriticalSection(&g_cs);
		return;
	}*/
	Py_XDECREF(_ref_data_list.ptr());
	DeleteCriticalSection(&g_cs);
	Py_FinalizeEx();
};

PythonImpl& PythonImpl::GetInstance()
{
	if (!s_instance)
	{
		try
		{
			s_instance = std::shared_ptr<PythonImpl>(new  PythonImpl());
		}
		catch (std::bad_alloc)
		{
			return *std::shared_ptr<PythonImpl>(nullptr);
		}
	}
	return *s_instance;
}

IPython* PythonFactory::GetPython()
{
	return &PythonImpl::GetInstance();
}

void* PythonImpl::Process(const wchar_t* module, const wchar_t* method, int data_type, int out_data_type,
	size_t input_dimensions, void * data, OUT size_t &output_dimensions,
	size_t input_size[], OUT size_t output_size[], bool is_need_ref_data)
{
	MyCriticalSection cs;
	if (is_need_ref_data && _ref_data == nullptr)
	{
		return nullptr;
	}

	if (!Py_IsInitialized())
	{
		throw L"Python cannot be Initialized correctly!";
	}

	try
	{
		bpy::object main_module = bpy::import("__main__");
		bpy::object main_namespace = main_module.attr("__dict__");
		bpy::object simple = bpy::exec_file(ToMbs(module).c_str(), main_namespace, main_namespace);
		bpy::object func = main_namespace[ToMbs(method).c_str()];

		// convert T* to python list
		bpy::list in_data_list;
		in_data_list = CArray2Pylist(data, data_type, input_dimensions, input_size);

		bpy::list return_list;
		switch (input_dimensions)
		{
		case 1:
			return_list = is_need_ref_data ? (bpy::extract<bpy::list>(func(in_data_list, _ref_data_list, input_size[0])))
				: (bpy::extract<bpy::list>(func(in_data_list, input_size[0])));
			break;
		case 2:
			return_list = is_need_ref_data ? (bpy::extract<bpy::list>(func(in_data_list, _ref_data_list, input_size[0], input_size[1])))
				: (bpy::extract<bpy::list>(func(in_data_list, input_size[0], input_size[1])));
			break;
		case 3:
			return_list = is_need_ref_data ? (bpy::extract<bpy::list>(func(in_data_list, _ref_data_list, input_size[0], input_size[1], input_size[2])))
				: (bpy::extract<bpy::list>(func(in_data_list, input_size[0], input_size[1], input_size[2])));
			break;
		case 4:
			return_list = is_need_ref_data ? (bpy::extract<bpy::list>(func(in_data_list, _ref_data_list, input_size[0], input_size[1], input_size[2], input_size[3])))
				: (bpy::extract<bpy::list>(func(in_data_list, input_size[0], input_size[1], input_size[2], input_size[3])));
			break;
		default:
			return nullptr;
		}

		size_t output_dims = bpy::extract<int>(return_list[0]);
		if (PyList_Size(return_list.ptr()) == output_dims + 2)
		{
			bpy::list out_data_list = bpy::extract<bpy::list>(return_list[1]);
			if (!PyList_Check(out_data_list.ptr()))
				return nullptr;
			for (size_t i = 0; i < output_dims; ++i)
			{
				output_size[i] = bpy::extract<size_t>(return_list[i + 2]);
			}

			void* output_data; //  = new OUT_T[GetTotalSize(output_dims, output_size)];
			output_data = Pylist2CArray(out_data_list, out_data_type, output_dims, output_size);
			output_dimensions = output_dims;
			return output_data;
		}
		else
		{
			// 无返回数据，该函数返回的返回值output_data的指针也自然为nullptr。
			// 或者返回数据格式出错！
			return nullptr;
		}
	}
	catch (bpy::error_already_set const&)
	{
		PyErr_Print();
		PyErr_Clear();
	}
	output_dimensions = 0;
	return nullptr;
};

void PythonImpl::SetRefData(void * ref_data, int data_type, size_t dimension_count, size_t size[])
{
	if (ref_data == nullptr)
		return;
	if (_ref_data != nullptr)
		return;

	_ref_data = ref_data;
	try
	{
		_ref_data_list = CArray2Pylist(_ref_data, data_type, dimension_count, size);
	}
	catch (bpy::error_already_set const&)
	{
		PyErr_Print();
		PyErr_Clear();
	}
}

void PythonImpl::DeleteRefData()
{
	_ref_data = nullptr;
}
