// PipelineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winbase.h>
#include <iostream>
#include "../Interface/YapInterfaces.h"
#include "../Interface/DataImp.h"
#include <string>
#include <vector>

using namespace std;
using namespace Yap;

#include "ProcessorDebugger.h"
#include "../PipelineConstructor/PipelineConstructor.h"
#include "../PipelineConstructor/CompositeProcessor.h"
#include "../PipelineConstructor/PipelineCompiler.h"

void ConstructorTest()
{
	try
	{
		CPipelineConstructor constructor;
		constructor.Reset(true);
		constructor.LoadModule(L"BasicRecon.dll");
		constructor.CreateProcessor(L"CmrRawDataReader", L"reader");
		constructor.SetProperty(L"reader", L"DataPath",
			L".\\Data\\HeadGibbs\\ExportCmrData-20151224\\MriRawData\\1.3.6.1.4.1.31636.24815489.151224005\\1");

		constructor.SetProperty(L"reader", L"ChannelCount", L"4");
		constructor.SetProperty(L"reader", L"ChannelSwitch", L"15"); // use all four channels.

		constructor.CreateProcessor(L"SliceIterator", L"slice_iterator");
		constructor.CreateProcessor(L"RemoveDC", L"dc_remover");
		constructor.CreateProcessor(L"ZeroFilling", L"zero_filling");
		constructor.CreateProcessor(L"Fft2D", L"fft");
		constructor.CreateProcessor(L"ModulePhase", L"module_phase");

		constructor.CreateProcessor(L"JpegExporter", L"jpeg_exporter");
		constructor.SetProperty(L"jpeg_exporter", L"ExportFolder", L"d:\\output");

		constructor.Link(L"reader", L"slice_iterator");
		constructor.Link(L"slice_iterator", L"dc_remover");
		constructor.Link(L"dc_remover", L"zero_filling");
		constructor.Link(L"zero_filling", L"fft");
		constructor.Link(L"fft", L"module_phase");
		constructor.Link(L"module_phase", L"Module", L"jpeg_exporter", L"Input");

		constructor.AssignPipelineInPort(L"Input", L"reader", L"Input");

		shared_ptr<IProcessor> pipeline = constructor.GetPipeline();
		if (pipeline)
		{
			pipeline->Input(L"Input", nullptr);
		}
	}
	catch (CConstructError& e)
	{
		wcout << e.GetErrorMessage() << std::endl;
	}
}

int main()
{
	// CProcessorDebugger debugger;
	// debugger.DebugPlugin(L"BasicRecon.dll");

	// ConstructorTest();

	CPipelineCompiler compiler;
	auto pipeline = compiler.CompileFile(L"test.pipeline");
	if (pipeline)
	{
		pipeline->Input(L"Input", nullptr);
	}

    return 0;
}

