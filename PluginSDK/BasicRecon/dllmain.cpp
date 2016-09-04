// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "../../Shared/Interface/Implement/ContainerImpl.h"
#include "Fft1D.h"
#include "Fft2D.h"
#include "ZeroFilling.h"
#include "ComplexSplitter.h"
#include "Grappa.h"
#include "ModulePhase.h"
#include "RemoveDC.h"
#include "CmrDataReader.h"
#include "JpegExporter.h"
#include "SliceIterator.h"
#include "imageProcessing.h"
#include "Algorithm2DWrapper.h"
#include "SliceSelector.h"
#include "../../Shared/Interface\Implement\YapImplement.h"

using namespace Yap;

BEGIN_DECL_PROCESSORS
	ADD_PROCESSOR(CSliceIterator)
	ADD_PROCESSOR(CRemoveDC)
	ADD_PROCESSOR(CFft1D)
	ADD_PROCESSOR(CFft2D)
	ADD_PROCESSOR(CZeroFilling)
	ADD_PROCESSOR(CComplexSplitter)
	ADD_PROCESSOR(CGrappa)
	ADD_PROCESSOR(CModulePhase)
	ADD_PROCESSOR(CJpegExporter)
	ADD_PROCESSOR(CSliceSelector)
	ADD(L"HFlipFloat", new CInPlaceAlgorithm2DWrapper<float>(hflip<float>, L"HFlipFloat"))
END_DECL_PROCESSORS

BOOL APIENTRY DllMain( HMODULE hModule,
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
