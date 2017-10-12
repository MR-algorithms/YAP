// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Implement/ContainerImpl.h"
#include "Algorithm2DWrapper.h"
#include "CalcuArea.h"
#include "ChannelDataCollector.h"
#include "ChannelIterator.h"
#include "ChannelMerger.h"
#include "CmrDataReader.h"
#include "ComplexSplitter.h"
#include "DataTypeConvertor.h"
#include "DcRemover.h"
#include "Difference.h"
#include "Fft1D.h"
#include "Fft2D.h"
#include "FineCF.h"
#include "imageProcessing.h"
#include "JpegExporter.h"
#include "ModulePhase.h"
#include "NiumagFidReader.h"
#include "NiumagFidWriter.h"
#include "NiumagImgReader.h"
#include "NiuMriImageReader.h"
#include "NiuMriImageWriter.h"
#include "NLM.h"
#include "Nlmeans.h"
#include "SamplingMaskCreator.h"
#include "SliceIterator.h"
#include "SliceMerger.h"
#include "SliceSelector.h"
#include "SubSampling.h"
#include "ZeroFilling.h"

#include "Implement/LogUserImpl.h"
#include "Implement/YapImplement.h"

using namespace Yap;

extern "C" {
	__declspec(dllexport) ILogUser* GetLogUser() 
	{
		return (ILogUser*) &LogUserImpl::GetInstance();
	}
};


typedef Python2DWrapper<unsigned short, unsigned short> Python2DWrapperUShortUShort;
typedef Python2DWrapper<float, float> Python2DWrapperFloatFloat;

BEGIN_DECL_PROCESSORS
	ADD_PROCESSOR(CalcuArea)
	ADD_PROCESSOR(ChannelDataCollector)
	ADD_PROCESSOR(ChannelIterator)
	ADD_PROCESSOR(ChannelMerger)
	ADD_PROCESSOR(CmrDataReader)
	ADD_PROCESSOR(ComplexSplitter)
	ADD_PROCESSOR(DataTypeConvertor)
	ADD_PROCESSOR(DcRemover)
	ADD_PROCESSOR(Difference)
	ADD_PROCESSOR(Fft1D)
	ADD_PROCESSOR(Fft2D)
	ADD_PROCESSOR(FineCF)
	ADD_PROCESSOR(JpegExporter)
	ADD_PROCESSOR(ModulePhase)
	ADD_PROCESSOR(NiumagFidReader)
	ADD_PROCESSOR(NiumagFidWriter)
	ADD_PROCESSOR(NiumagImgReader)
	ADD_PROCESSOR(NiuMriImageReader)
	ADD_PROCESSOR(NiuMriImageWriter)
	ADD_PROCESSOR(NLM)
	ADD_PROCESSOR(Nlmeans)
	ADD_PROCESSOR(SamplingMaskCreator)
	ADD_PROCESSOR(SliceIterator)
	ADD_PROCESSOR(SliceMerger)
	ADD_PROCESSOR(SliceSelector)
	ADD_PROCESSOR(SubSampling)
	ADD_PROCESSOR(ZeroFilling)
	ADD(L"HFlipFloat", new Algorithm2DInPlaceWrapper<float>(hflip<float>, L"HFlipFloat"))
	ADD(L"PythonUSimShow", new Python2DWrapperUShortUShort(L"Py2C", L"ShowImage", L"PythonUSimShow"))
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
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
