// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Implement\LogUserImpl.h"
#include "Implement\YapImplement.h"

#include "FileIterator.h"
#include "FolderIterator.h"
#include "NiiReader.h"
#include "ExtractFeatures.h"
#include "RFeaturesCollector.h"
#include "Implement\PythonUserImpl.h"
#include "FolderCollector.h"
#include "PyClassification.h"

using namespace Yap;
extern "C" {
	_declspec(dllexport) IPythonUser * GetPythonUser()
	{
		return &PythonUserImpl::GetInstance();
	}
	_declspec(dllexport) ILogUser* GetLogUser()
	{
		return (ILogUser*)&LogUserImpl::GetInstance();
	}
};

BEGIN_DECL_PROCESSORS
	ADD_PROCESSOR(FolderCollector)
	ADD_PROCESSOR(FileIterator)
	ADD_PROCESSOR(FolderIterator)
	ADD_PROCESSOR(NiiReader)
	ADD_PROCESSOR(ExtractFeatures)
	ADD_PROCESSOR(RFeaturesCollector)
	ADD_PROCESSOR(PyClassification)
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
