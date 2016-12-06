#pragma once

#ifndef Algorithm2DWrapper_h__20160813
#define Algorithm2DWrapper_h__20160813


#include "Interface/Implement/ProcessorImpl.h"
#include <complex>

namespace Yap
{
	template<typename INPUT_TYPE, typename OUTPUT_TYPE>
	class Algorithm2DWrapper :
		public ProcessorImpl
	{
	public:
		typedef void (*ProcessingFunc) (INPUT_TYPE * input_data, OUTPUT_TYPE * output_data, 
			size_t width, size_t height);	

		explicit Algorithm2DWrapper(ProcessingFunc func, const wchar_t * processor_name) :
			_func(func), ProcessorImpl(processor_name)
		{
			AddInput(L"Input", 2, type_id<INPUT_TYPE>::type);
			AddOutput(L"Output", 2, type_id<OUTPUT_TYPE>::type);
		}

		Algorithm2DWrapper(const Algorithm2DWrapper<INPUT_TYPE, OUTPUT_TYPE>& rhs) :
			_func(rhs._func), ProcessorImpl(rhs)
		{
		}

		virtual bool Input(const wchar_t * port, IData * data) override
		{
			if (wstring(port) != L"Input")
				return false;

			if (data->GetDataType() != type_id<INPUT_TYPE>::type)
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

		virtual IProcessor * Clone() override
		{
			return new(nothrow) Algorithm2DWrapper<INPUT_TYPE, OUTPUT_TYPE>(*this);
		}
	protected:
		~Algorithm2DWrapper() {}

		ProcessingFunc _func;
	};


	template<typename T>
	class Algorithm2DInPlaceWrapper :
		public ProcessorImpl
	{
	public:
		typedef void(*ProcessingFunc) (T * input_data, size_t width, size_t height);

		explicit Algorithm2DInPlaceWrapper(ProcessingFunc func, const wchar_t * processor_name) :
			_func(func), ProcessorImpl(processor_name)
		{
			AddInput(L"Input", 2, type_id<T>::type);
			AddOutput(L"Output", 2, type_id<T>::type);
		}

		Algorithm2DInPlaceWrapper(const Algorithm2DInPlaceWrapper<T>& rhs) :
			_func(rhs._func), ProcessorImpl(rhs)
		{
		}

		virtual bool Input(const wchar_t * port, IData * data) override
		{
			if (std::wstring(port) != L"Input")
				return false;

			if (data->GetDataType() != type_id<T>::type)
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

		virtual IProcessor * Clone() override
		{
			return new (std::nothrow) Algorithm2DInPlaceWrapper<T>(*this);
		}
	protected:
		~Algorithm2DInPlaceWrapper() {}

		ProcessingFunc _func;
	};
}


#endif // Algorithm2DWrapper_h__
