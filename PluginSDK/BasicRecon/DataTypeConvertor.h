#pragma once

#ifndef DataTypeConvertor_h__201609
#define DataTypeConvertor_h__201609

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{
	class DataTypeConvertor :
		public ProcessorImpl
	{
	public:
		DataTypeConvertor(void);
		DataTypeConvertor(const DataTypeConvertor& rhs);
		~DataTypeConvertor();

		virtual bool Input(const wchar_t * port, IData * data) override;

		virtual IProcessor * Clone() override;

		// Inherited via ProcessorImpl
		virtual bool OnInit() override;

	protected:
		int GetOutputDataType();
		const wchar_t * GetPortName(int data_type);
		static std::map<int, wchar_t *> s_data_type_to_port;
	};
}

#endif // DataTypeConvertor_h__