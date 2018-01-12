#include "PythonImpl.h"
#include <windows.h>

// dllmain.cpp : Defines the entry point for the DLL application.

using namespace Yap;

#ifdef _cplusplus
extern "C" {
#endif

#define PYRECON_EXTERN extern _declspec(dllexport)

	PYRECON_EXTERN IPython * GetPython()
	{
		return PythonFactory::GetPython();
	}

#ifdef _cplusplus
};
#endif // _cplusplus

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
