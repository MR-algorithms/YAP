#pragma once

#ifndef Algorithm2DWrapper_h__20160813
#define Algorithm2DWrapper_h__20160813


#include "Interface/Implement/ProcessorImpl.h"
#include <complex>

namespace Yap
{
	template<typename INPUT_TYPE, typename OUTPUT_TYPE>
	class CAlgorithm2DWrapper :
		public ProcessorImpl
	{
	public:
		typedef void (*ProcessingFunc) (INPUT_TYPE * input_data, OUTPUT_TYPE * output_data, 
			size_t width, size_t height);	

		explicit CAlgorithm2DWrapper(ProcessingFunc func, const wchar_t * processor_name) :
			_func(func), ProcessorImpl(processor_name)
		{
			AddInputPort(L"Input", 2, type_id<INPUT_TYPE>::type);
			AddOutputPort(L"Output", 2, type_id<OUTPUT_TYPE>::type);
		}

		CAlgorithm2DWrapper(const CAlgorithm2DWrapper<INPUT_TYPE, OUTPUT_TYPE>& rhs) :
			_func(rhs._func)
		{
		}

		~CAlgorithm2DWrapper() {}

		virtual bool Input(const wchar_t * port, IData * data) override
		{
			if (wstring(port) != L"Input")
				return false;

			if (data->GetDataType() != type_id<INPUT_TYPE>)
				return false;

			CDataHelper input_data(data);
			if (input_data.GetActualDimensionCount() != 2)
				return false;

			unsigned int width = input_data.GetWidth();
			unsigned int height = input_data.GetHeight();

			auto output_data = CSmartPtr<CDataImp<OUTPUT_TYPE>>(new CDataImp<OUTPUT_TYPE>(data->GetDimensions()));

			_func(reinterpret_cast<INPUT_TYPE *>(input_data.GetData()),
				reinterpret_cast<OUTPUT_TYPE *>(output_data->GetData()),
				width, height);
			Feed(L"Output", output_data.get());
		}

		virtual IProcessor * Clone() override
		{
			return new CAlgorithm2DWrapper<INPUT_TYPE, OUTPUT_TYPE>(*this);
		}
	protected:
		ProcessingFunc _func;
	};


	template<typename T>
	class CInPlaceAlgorithm2DWrapper :
		public ProcessorImpl
	{
	public:
		typedef void(*ProcessingFunc) (T * input_data, size_t width, size_t height);

		explicit CInPlaceAlgorithm2DWrapper(ProcessingFunc func, const wchar_t * processor_name) :
			_func(func), ProcessorImpl(processor_name)
		{
			AddInputPort(L"Input", 2, type_id<T>::type);
			AddOutputPort(L"Output", 2, type_id<T>::type);
		}

		CInPlaceAlgorithm2DWrapper(const CInPlaceAlgorithm2DWrapper<T>& rhs) :
			_func(rhs._func), ProcessorImpl(rhs)
		{
		}

		~CInPlaceAlgorithm2DWrapper() {}

		virtual bool Input(const wchar_t * port, IData * data) override
		{
			if (std::wstring(port) != L"Input")
				return false;

			if (data->GetDataType() != type_id<T>::type)
				return false;

			CDataHelper input_data(data);
			if (input_data.GetActualDimensionCount() != 2)
				return false;

			unsigned int width = input_data.GetWidth();
			unsigned int height = input_data.GetHeight();

			_func(reinterpret_cast<T *>(input_data.GetData()), width, height);
			Feed(L"Output", data);

			return true;
		}

		virtual IProcessor * Clone() override
		{
			return new CInPlaceAlgorithm2DWrapper<T>(*this);
		}
	protected:
		ProcessingFunc _func;
	};
}


#endif // Algorithm2DWrapper_h__
