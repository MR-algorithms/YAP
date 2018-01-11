#ifndef PythonImpl_H_20171020
#define PythonImpl_H_20171020

#pragma once
#include "Interface\IPython.h"

//#ifdef _DEBUG
//	#ifdef _WIN64
//		#pragma comment(lib, "PythonImplement64D")
//	#else
//		#pragma comment(lib, "PythonImplementD")
//	#endif
//#else    // release version
//	#ifdef _WIN64
//		#pragma comment(lib, "PythonImplement64")
//	#else
//		#pragma comment(lib, "PythonImplement")
//	#endif
//#endif

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