#pragma once

#ifndef DataTypeConvertor_h__201609
#define DataTypeConvertor_h__201609

#include "Implement/ProcessorImpl.h"

namespace Yap
{
	class DataTypeConvertor :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(DataTypeConvertor)
	public:
		DataTypeConvertor(void);
		DataTypeConvertor(const DataTypeConvertor& rhs);

	protected:
		~DataTypeConvertor();

		virtual bool Input(const wchar_t * port, IData * data) override;

		int GetOutputDataType();
		template <typename IN_TYPE, typename OUT_TYPE>
		Yap::SmartPtr<IData> GetConvertedData(IData* input_object);
		template <typename IN_TYPE>
		Yap::SmartPtr<IData> Convert(IData * input, int output_type);

		const wchar_t * GetPortName(int data_type);
		static std::map<int, wchar_t *> s_data_type_to_port;
	};
}

#endif // DataTypeConvertor_h__