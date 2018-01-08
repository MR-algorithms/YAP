#pragma once

#ifndef PythonUserImpl_H_20171020
#define PythonUserImpl_H_20171020

#include "Interface\IPythonUser.h"
#include <memory>

namespace Yap
{
	class PythonUserImpl : public IPythonUser {

	public:
		~PythonUserImpl();

		static PythonUserImpl& GetInstance();
		
		virtual void SetPython(IPython* python) override;
		
		IPython* GetPython() override;

	protected:
		PythonUserImpl();
		static std::shared_ptr<PythonUserImpl> s_instance;
		IPython * _python;

	};


}
#endif
