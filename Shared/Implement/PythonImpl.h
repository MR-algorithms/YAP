#ifndef PythonImpl_H_20171020
#define PythonImpl_H_20171020

#pragma once

#include <Interface/IPython.h>
#include <Windows.h>

namespace Yap
{
	class PythonImpl : public IPython {
	public:
		
		virtual void InitPython() override;

		virtual void ReleasePython() override;

		~PythonImpl();

		static PythonImpl& GetInstance();

	protected:
		PythonImpl();
		static std::shared_ptr<PythonImpl> s_instance;
		bool _initialzed;
	private:
		CRITICAL_SECTION cs;
	};

}

#endif