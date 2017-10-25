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

namespace Yap
{
	namespace PyDataType{
		//dynamic python arguments type convert define.
		template<typename T> struct pydata_type_id{ const char * type = "";};
		// as below is implement struct pydata_type_id.
		template<> struct pydata_type_id<double> { const char * type = "d"; };
		template<> struct pydata_type_id<char> { const char * type = "s"; };
		template<> struct pydata_type_id<unsigned char> { const char* type = "b"; };
		template<> struct pydata_type_id<short> { const char * type = "h"; };
		template<> struct pydata_type_id<unsigned short> { const char * type = "h"; };
		template<> struct pydata_type_id<float> { const char * type = "f"; };
		template<> struct pydata_type_id<unsigned int> { const char * type = "i"; };
		template<> struct pydata_type_id<int> { const char * type = "i"; };
		template<> struct pydata_type_id<std::complex<float>> { const char * type = "O&"; };// convert to Python object. if error, return NULL.
		template<> struct pydata_type_id<std::complex<double>> { const char * type = "O&"; };
		template<> struct pydata_type_id<bool> { const char * type = "i"; };
	};

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
		~Algorithm2DInPlaceWrapper() {}

		ProcessingFunc _func;
	};


	template<typename INPUT_TYPE, typename OUTPUT_TYPE>
	class Python2DWrapper :
		public ProcessorImpl
	{
		typedef Python2DWrapper<INPUT_TYPE, OUTPUT_TYPE> MyClass;
		IMPLEMENT_SHARED(MyClass)
	public:

		explicit Python2DWrapper(const wchar_t * module_file_name, const wchar_t * method_name, const wchar_t * processor_name):
			ProcessorImpl(processor_name)
		{
			autro python = dynamic_cast<PythonImpl*>(PythonUserImpl::GetInstance().GetPython());
			python->InitPython();
			
			assert(python->CheckScriptInfo(module_file_name, null, method_name) && l"no find python script!");
			

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
		}


		Python2DWrapper(const Python2DWrapper<INPUT_TYPE, OUTPUT_TYPE>& rhs) :
			ProcessorImpl(rhs)
		{
			// _pyobjects(rhs._pyobjects),
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

			DataHelper input_data(data);
			if (input_data.GetActualDimensionCount() != 2)
				return false;

			size_t width = static_cast<size_t>(input_data.GetWidth());
			size_t height = static_cast<size_t>(input_data.GetHeight());
			auto py_data = GetDataArray<INPUT_TYPE>(data);

			auto python = dynamic_cast<PythonImpl*>(PythonUserImpl::GetInstance().GetPython());
			assert(python);
			python->Python1DListInit(plist, py_data, width*height);

			//	add DataConvert c2py
			PyObject * plist = PyList_New(width*height);
			if (!python->CPyList_Check(plist))
				throw PyErr_NewException;

			//PyDataType::pydata_type_id<INPUT_TYPE> in_wchar;
			////add input_data to plist
			//for (size_t w = 0; w < width * height; ++w)
			//{
			//	PyObject *pl = Py_BuildValue(in_wchar.type, *(py_data + w));
			//	PyList_SetItem(plist, w, pl);
			//}
			python->PyTupleSetItem(pArgs, Py_ssize_t(0), plist);
			python->PyTupleSetItem(pArgs, Py_ssize_t(1), Py_BuildValue("i", width));
			python->PyTupleSetItem(pArgs, Py_ssize_t(1), Py_BuildValue("i", height));

			PyObject * pArgs = PyTuple_New(3);
			PyTuple_SetItem(pArgs, Py_ssize_t(0), plist);
			PyTuple_SetItem(pArgs, Py_ssize_t(1), Py_BuildValue("i",width));
			PyTuple_SetItem(pArgs, Py_ssize_t(2), Py_BuildValue("i",height));

			if (_pyobjects.find(L"Args") == _pyobjects.end())
			{
				_pyobjects.insert(std::make_pair(L"Args", pArgs));
			}
			else
			{
				_pyobjects[L"Args"] = pArgs;
			}
			auto func = _pyobjects.at(L"Func");
			// add handle method
			PyObject * pRet = PyEval_CallObject(func, pArgs);

			// add DataConvert py2c

			showResult(pRet);

			//Yap::IData * out_data = NULL;
			// CDataImp<OUTPUT_TYPE> * output_data = NULL;
			//Feed(L"Output", out_data); //output_data.get()
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
		~Python2DWrapper()
		{
			// release Python resource
			/*for (auto iter : _pyobjects)
			{
				Py_DECREF(iter.second);
			}
			_pyobjects.clear();
			Py_Finalize();*/
		}
		//std::map <const wchar_t*, PyObject*> _pyobjects;
	};

}

#endif // Algorithm2DWrapper_h__
