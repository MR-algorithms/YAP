// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include "Implement/ContainerImpl.h"
#include "Algorithm2DWrapper.h"
#include "CalcuArea.h"
#include "ChannelDataCollector.h"
#include "ChannelImageDataCollector.h"
#include "ChannelIterator.h"
#include "ChannelMerger.h"
#include "CmrDataReader.h"
#include "ComplexSplitter.h"
#include "DataTypeConvertor.h"
#include "DcRemover.h"
#include "Difference.h"
#include "Fft1D.h"
#include "Fft2D.h"
#include "Fft3D.h"
#include "FineCF.h"
#include "GrayScaleUnifier.h"
#include "imageProcessing.h"
#include "JpegExporter.h"
#include "LinesSelector.h"
#include "ModulePhase.h"
#include "NiumagFidReader.h"
#include "NiumagFidWriter.h"
#include "NiumagImgReader.h"
#include "NiumagPFFTConjugator.h"
#include "NiuMriImageReader.h"
#include "NiuMriImageWriter.h"
#include "NLM.h"
#include "Nlmeans.h"
#include "PhaseCorrector.h"
#include "SamplingMaskCreator.h"
#include "SliceIterator.h"
#include "SliceMerger.h"
#include "SliceSelector.h"
#include "ChannelSelector.h"
#include "SubSampling.h"
#include "RadialSampling.h"
#include "ZeroFilling.h"
#include "DeOversampling.h"
#include "SimpleGrid.h"
#include "DrawlineOnImage.h"
#include "Flip.h"

//#include "IceReceiver.h"

#include "IcePeFt.h"

#include "Implement/LogUserImpl.h"
#include "Implement/YapImplement.h"

using namespace Yap;

extern "C" { 
	__declspec(dllexport) ILogUser* GetLogUser() 
	{
		return (ILogUser*) &LogUserImpl::GetInstance();
	}
};

BEGIN_DECL_PROCESSORS
	ADD_PROCESSOR(CalcuArea)
	ADD_PROCESSOR(ChannelDataCollector)
	ADD_PROCESSOR(ChannelImageDataCollector)
	ADD_PROCESSOR(ChannelIterator)
	ADD_PROCESSOR(ChannelMerger)
	ADD_PROCESSOR(CmrDataReader)
	ADD_PROCESSOR(ComplexSplitter)
	ADD_PROCESSOR(DataTypeConvertor)
	ADD_PROCESSOR(DcRemover)
	ADD_PROCESSOR(Difference)
	ADD_PROCESSOR(Fft1D)
	ADD_PROCESSOR(Fft2D)
	ADD_PROCESSOR(Fft3D)
	ADD_PROCESSOR(FineCF)
	ADD_PROCESSOR(GrayScaleUnifier)
	ADD_PROCESSOR(JpegExporter)
	ADD_PROCESSOR(LinesSelector)
	ADD_PROCESSOR(ModulePhase)
	ADD_PROCESSOR(NiumagFidReader)
	ADD_PROCESSOR(NiumagFidWriter)
	ADD_PROCESSOR(NiumagImgReader)
	ADD_PROCESSOR(NiumagPFFTConjugator)
	ADD_PROCESSOR(NiuMriImageReader)
	ADD_PROCESSOR(NiuMriImageWriter)
	ADD_PROCESSOR(NLM)
	ADD_PROCESSOR(Nlmeans)
	ADD_PROCESSOR(PhaseCorrector)
	ADD_PROCESSOR(SamplingMaskCreator)
	ADD_PROCESSOR(SliceIterator)
	ADD_PROCESSOR(SliceMerger)
	ADD_PROCESSOR(SliceSelector)
	ADD_PROCESSOR(ChannelSelector)
	ADD_PROCESSOR(SubSampling)
	ADD_PROCESSOR(RadialSampling)
	ADD_PROCESSOR(DrawlineOnImage)
	ADD_PROCESSOR(ZeroFilling)
	ADD_PROCESSOR(DeOversampling)
	ADD_PROCESSOR(SimpleGrid)
	ADD_PROCESSOR(Flip)

	//ADD_PROCESSOR(IceReceiver)
	
	ADD_PROCESSOR(IcePeFt)
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
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
