// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Interface/Implement/ContainerImpl.h"
#include "Fft1D.h"
#include "Fft2D.h"
#include "ZeroFilling.h"
#include "ComplexSplitter.h"
#include "Grappa.h"
#include "ModulePhase.h"
#include "DcRemover.h"
#include "CmrDataReader.h"
#include "JpegExporter.h"
#include "SamplingMaskCreator.h"
#include "SliceIterator.h"
#include "imageProcessing.h"
#include "Algorithm2DWrapper.h"
#include "SliceSelector.h"
#include "Difference.h"

#include "Interface\Implement\YapImplement.h"

using namespace Yap;

BEGIN_DECL_PROCESSORS
	ADD_PROCESSOR(CmrDataReader)
	ADD_PROCESSOR(ComplexSplitter)
	ADD_PROCESSOR(Difference)
	ADD_PROCESSOR(Fft1D)
	ADD_PROCESSOR(Fft2D)
	ADD_PROCESSOR(Grappa)
	ADD_PROCESSOR(JpegExporter)
	ADD_PROCESSOR(ModulePhase)
	ADD_PROCESSOR(DcRemover)
	ADD_PROCESSOR(SamplingMaskCreator)
	ADD_PROCESSOR(SliceIterator)
	ADD_PROCESSOR(SliceSelector)
	ADD_PROCESSOR(ZeroFilling)
	ADD(L"HFlipFloat", new Algorithm2DInPlaceWrapper<float>(hflip<float>, L"HFlipFloat"))
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
