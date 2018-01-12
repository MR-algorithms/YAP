// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Implement\LogUserImpl.h"
#include "Implement\YapImplement.h"

#include "FilesIterator.h"
#include "FolderIterator.h"
#include "NiiReader.h"
#include "Radiomics.h"
#include "RFeaturesCollector.h"
#include "Implement\PythonUserImpl.h"

using namespace Yap;
#ifdef _cplusplus
extern "C" {
#endif
#define PYRECON_EXTERN extern _declspec(dllexport)

	PYRECON_EXTERN IPythonUser * GetPythonUser()
	{
		return &PythonUserImpl::GetInstance();
	}
	PYRECON_EXTERN ILogUser* GetLogUser()
	{
		return (ILogUser*)&LogUserImpl::GetInstance();
	}

#ifdef _cplusplus
};
#endif // _cplusplus

BEGIN_DECL_PROCESSORS
	ADD_PROCESSOR(FilesIterator)
	ADD_PROCESSOR(FolderIterator)
	ADD_PROCESSOR(NiiReader)
	ADD_PROCESSOR(Radiomics)
	ADD_PROCESSOR(RFeaturesCollector)
END_DECL_PROCESSORS

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
