#pragma once
#include "Implement\ProcessorImpl.h"
#include "Interface\smartptr.h"

namespace Yap
{
	class FineCF :
		public ProcessorImpl
	{
		IMPLEMENT_SHARED(FineCF)
	public:
		FineCF();
		FineCF(const FineCF& rhs);

	protected:
		~FineCF();

		virtual bool Input(const wchar_t * port, IData * data) override;

		// property input O1, SW
		// property output CF;
	private:
	};

}

