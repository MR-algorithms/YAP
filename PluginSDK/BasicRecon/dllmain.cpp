// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Tools/ProcessorManagerImp.h"
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

using namespace Yap;

extern "C"
{
	__declspec(dllexport) IProcessorManager * GetProcessorManager()
	{
		auto processor_manager = new CProcessorManagerImp;

		processor_manager->AddProcessor(new CCmrDataReader);
		processor_manager->AddProcessor(new CSliceIterator);
		processor_manager->AddProcessor(new CRemoveDC);
		processor_manager->AddProcessor(new CFft1D);
		processor_manager->AddProcessor(new CFft2D);
		processor_manager->AddProcessor(new CZeroFilling);
		processor_manager->AddProcessor(new CComplexSplitter);
		processor_manager->AddProcessor(new CGrappa);
		processor_manager->AddProcessor(new CModulePhase);
		processor_manager->AddProcessor(new CJpegExporter);
		processor_manager->AddProcessor(new CSliceSelector);

		processor_manager->AddProcessor(new CInPlaceAlgorithm2DWrapper<float>(hflip<float>, L"HFlipFloat"));
		 
		return processor_manager;
	}
}

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
