#pragma once

#ifndef ProcessorManagerAgent_h__20160813
#define ProcessorManagerAgent_h__20160813

#include "Interface/Interfaces.h"
#include <wtypes.h>

#include "PythonImplement\PythonImpl.h" 
#ifdef _DEFINE_PYTHON
#define SET_PYTHON_2USER(_module) \
{	\
	auto python_func = (Yap::IPythonUser*(*)())::GetProcAddress(_module, "GetPythonUser"); \
	if (python_func != nullptr)\
	{\
		auto python_user = python_func(); \
		if (python_user != nullptr)\
		{\
			python_user->SetPython(PythonFactory::GetPython()); \
		}\
	}\
}
#else
#define SET_PYTHON_2USER(_module) 
#endif // _DEFINE_PYTHON

namespace Yap
{
	class ModuleAgent
	{
	public:
		ModuleAgent();

		bool Load(const wchar_t * plugin_path);
		IProcessor * Find(const wchar_t * name);
		IProcessorIter * GetIterator();
		bool Add(const wchar_t * name, IProcessor * processor);

		~ModuleAgent();

	protected:

		HINSTANCE _module;
		SmartPtr<IProcessorContainer> _manager;
	};

}
#endif // ProcessorManagerAgent_h__
