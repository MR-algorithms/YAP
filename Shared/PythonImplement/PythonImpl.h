#ifndef PythonImpl_H_20171020
#define PythonImpl_H_20171020

#pragma once
#include "Interface\IPython.h"

// struct IPython;

namespace Yap
{
	class PythonFactory
	{
	public:
		static IPython* GetPython();
	};
}
#endif