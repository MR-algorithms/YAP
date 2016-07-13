#pragma once
#include "ProcessorImp.h"
#include <vector>

namespace Yap
{
	class CZeroFilling :
		public CProcessorImp
	{
	public:
		CZeroFilling();

		virtual bool Input(const wchar_t * port, IData * data) override;

	protected:

		virtual ~CZeroFilling();

		virtual IProcessor * Clone() override;

	};

}
