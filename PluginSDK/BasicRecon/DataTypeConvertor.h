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
	};
}

#endif // DataTypeConvertor_h__