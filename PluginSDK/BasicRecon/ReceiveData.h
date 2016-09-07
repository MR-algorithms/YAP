#pragma once
#ifndef ReceiveData_h_20160907
#define ReceiveData_h_20160907

#include "Interface/Implement/ProcessorImpl.h"

namespace Yap
{

	class ReceiveData :
		public ProcessorImpl
	{
	public:
		ReceiveData();
		virtual ~ReceiveData();

		virtual bool OnInit() override;

		virtual IProcessor * Clone() override;

		virtual bool Input(const wchar_t * name, IData * data) override;

	};

}

#endif