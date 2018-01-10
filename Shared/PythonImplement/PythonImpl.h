#ifndef PythonImpl_H_20171020
#define PythonImpl_H_20171020

#pragma once
#include "Interface\IPython.h"

#ifndef _DEFINE_PYTHON
#define _DEFINE_PYTHON
#endif // !_DEFINE_PYTHON


// struct IPython;

namespace Yap
{
	class PythonFactory
	{
	public:
		static IPython& GetPython();
	};
}
#endif