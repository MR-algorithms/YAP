#include "stdafx.h"
#include "Interface/Implement/ContainerImpl.h"
#include "Interface\Implement\YapImplement.h"
#include "cuFft2D.h"


using namespace Yap;

BEGIN_DECL_PROCESSORS
ADD_PROCESSOR(cuFft2D)
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
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
