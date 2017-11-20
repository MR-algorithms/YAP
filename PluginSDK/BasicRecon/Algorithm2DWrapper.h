#pragma once

#ifndef Algorithm2DWrapper_h__20160813
#define Algorithm2DWrapper_h__20160813

#include <Python.h>
#include "Implement/ProcessorImpl.h"
#include <complex>
#include <map>
#include <comdef.h>
#include <stdio.h>
#include "Implement/PythonImpl.h"
#include "Implement/PythonUserImpl.h"
#include <assert.h>
#include <iostream>
#include "Implement/LogUserImpl.h"

namespace Yap
{
	template<typename INPUT_TYPE, typename OUTPUT_TYPE>
	class Algorithm2DWrapper :
		public ProcessorImpl
	{
		typedef Algorithm2DWrapper<INPUT_TYPE, OUTPUT_TYPE> this_class;
		IMPLEMENT_SHARED(this_class)
	public:
		typedef void (*ProcessingFunc) (INPUT_TYPE * input_data, OUTPUT_TYPE * output_data, 
			size_t width, size_t height);

		explicit Algorithm2DWrapper(ProcessingFunc func, const wchar_t * processor_name):
			_func(func), ProcessorImpl(processor_name)
		{
			AddInput(L"Input", 2, data_type_id<INPUT_TYPE>::type);
			AddOutput(L"Output", 2, data_type_id<OUTPUT_TYPE>::type);
		}


		Algorithm2DWrapper(const Algorithm2DWrapper<INPUT_TYPE, OUTPUT_TYPE>& rhs):
			_func(rhs._func), ProcessorImpl(rhs)
		{
		}

		virtual bool Input(const wchar_t * port, IData * data) override
		{
			if (wstring(port) != L"Input")
				return false;

			if (data->GetDataType() != data_type_id<INPUT_TYPE>::type)
				return false;

			DataHelper input_data(data);
			if (input_data.GetActualDimensionCount() != 2)
				return false;

			unsigned int width = input_data.GetWidth();
			unsigned int height = input_data.GetHeight();

			auto output_data = CSmartPtr<CDataImp<OUTPUT_TYPE>>(new CDataImp<OUTPUT_TYPE>(data->GetDimensions()));

			_func(GetDataArray<INPUT_TYPE>(data),
				GetDataArray<OUTPUT_TYPE(output_data.get()),
				width, height);
			Feed(L"Output", output_data.get());
		}

	protected:
		~Algorithm2DWrapper() 
		{
		}

		ProcessingFunc _func;
	};

	template<typename T>
	class Algorithm2DInPlaceWrapper :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(Algorithm2DInPlaceWrapper<T>)
	public:
		typedef void(*ProcessingFunc) (T * input_data, size_t width, size_t height);

		explicit Algorithm2DInPlaceWrapper(ProcessingFunc func, const wchar_t * processor_name) :
			_func(func), ProcessorImpl(processor_name)
		{
			AddInput(L"Input", 2, data_type_id<T>::type);
			AddOutput(L"Output", 2, data_type_id<T>::type);
		}

		Algorithm2DInPlaceWrapper(const Algorithm2DInPlaceWrapper<T>& rhs):
			_func(rhs._func), ProcessorImpl(rhs)
		{

		}

		virtual bool Input(const wchar_t * port, IData * data) override
		{
			if (std::wstring(port) != L"Input")
				return false;

			if (data->GetDataType() != data_type_id<T>::type)
				return false;

			DataHelper input_data(data);
			if (input_data.GetActualDimensionCount() != 2)
				return false;

			unsigned int width = input_data.GetWidth();
			unsigned int height = input_data.GetHeight();

			_func(GetDataArray<T>(data), width, height);
			Feed(L"Output", data);

			return true;
		}

	protected:
		~Algorithm2DInPlaceWrapper() {
			LOG_TRACE(L"",)
		}

		ProcessingFunc _func;
	};


	template<typename INPUT_TYPE, typename OUTPUT_TYPE>
	class Python2DWrapper :
		public ProcessorImpl
	{
		typedef Python2DWrapper<INPUT_TYPE, OUTPUT_TYPE> MyClass;
		IMPLEMENT_SHARED(MyClass)
	public:

		explicit Python2DWrapper(const wchar_t * processor_name)
			: _python(nullptr), ProcessorImpl(processor_name) 
		{
			LOG_TRACE(L"Python2DWrapper constructor called.", L"BasicRecon");
			_python = PythonUserImpl::GetInstance().GetPython();
			/*
				PyObject * pmodule = PyImport_ImportModule(_bstr_t(module_file_name));
				if (!pmodule)
					throw PyErr_SetImportError;

				PyObject * pfunc = PyObject_GetAttrString(pmodule, _bstr_t(method_name));

				if (!pfunc || !PyCallable_Check(pfunc))
					throw PyErr_GetExcInfo;


				if (!_pyobjects.empty())
				{
					for (auto iter : _pyobjects)
					{
						if (iter.second)
							Py_DECREF(iter.second);
					}
					_pyobjects.clear();
				}
				_pyobjects.insert(std::make_pair(l"module", pmodule));
				_pyobjects.insert(std::make_pair(l"func", pfunc));
			*/
			AddInput(L"Input", 2, data_type_id<INPUT_TYPE>::type);
			AddOutput(L"Output", 2, data_type_id<OUTPUT_TYPE>::type);
			AddProperty<const wchar_t * const>(L"ScriptPath", L"", L"包含Python脚本文件的文件夹。");
			AddProperty<const wchar_t * const>(L"MethodName", L"", L"脚本文件中需要使用的方法名");
		}


		Python2DWrapper(const Python2DWrapper<INPUT_TYPE, OUTPUT_TYPE>& rhs) :
			_python(rhs._python), ProcessorImpl(rhs) 
		{
		};

		/*
		@note convert C data to python-form data, and handle the function
			with python method, then return python value, finally, change
			python data to C data.
		*/
		virtual bool Input(const wchar_t * port, IData * data) override
		{
			if (std::wstring(port) != L"Input")
				return false;

			if (data->GetDataType() != data_type_id<INPUT_TYPE>::type)
				return false;

			DataHelper help_data(data);
			if (help_data.GetActualDimensionCount() != 2)
				return false;
			size_t width = static_cast<size_t>(help_data.GetWidth());
			size_t height = static_cast<size_t>(help_data.GetHeight());
			size_t out_width = 0, out_height = 0;

			auto input_data = GetDataArray<INPUT_TYPE>(data);
			//Get Properties
			const wchar_t* module_name = GetProperty<const wchar_t * const>(L"ScriptPath");
			const wchar_t* method_name = GetProperty<const wchar_t * const>(L"MethodName");

			OUTPUT_TYPE* out_data= reinterpret_cast<OUTPUT_TYPE*>(_python->Process2D(module_name, method_name,
				data_type_id<INPUT_TYPE>::type, input_data, width, height, out_width, out_height));
			
			// create output data
			Dimensions dimensions;
			dimensions(DimensionReadout, 0U, out_width)
				(DimensionPhaseEncoding, 0U, out_height);

			auto output_data = CreateData<OUTPUT_TYPE>(nullptr, out_data, dimensions, nullptr, true);
			Feed(L"Output", output_data.get());
			return true;
		}

		bool showResult(PyObject * pRet)
		{
			if (!pRet)
				throw PyErr_BadInternalCall;
			int status = 0;
			char * result;

			status = PyArg_Parse(pRet, "s", &result);
			if (pRet && status)
			{
				printf("Calling Python--Function [%s] result: %s\n", "ShowImage", result);
				return true;
			}
			else
			{
				printf("Can not get the return value!");
				return NULL;
			}
		}


	protected:
		~Python2DWrapper(){
			LOG_TRACE(L"Python2DWrapper destructor", L"BasicRecon");
		};

		IPython* _python;
	};

}

#endif // Algorithm2DWrapper_h__
