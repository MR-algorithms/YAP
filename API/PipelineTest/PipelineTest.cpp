// PipelineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ProcessorDebugger.h"
#include "Yap/PipelineConstructor.h"
#include "Yap/PipelineCompiler.h"
#include "Interface/Implement/CompositeProcessor.h"
#include "Interface/Implement/DataObject.h"

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "Yap/VdfParser.h"

using namespace std;
using namespace Yap;

void ConstructorTest()
{
	try
	{
		PipelineConstructor constructor;
		constructor.Reset(true);
		constructor.LoadModule(L"BasicRecon.dll");
		constructor.CreateProcessor(L"CmrRawDataReader", L"reader");
		constructor.SetProperty(L"reader", L"DataPath",
			L"D:\\test_data\\1.3.6.1.4.1.31636.24815489.151224005\\1");

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

		constructor.MapInput(L"Input", L"reader", L"Input");

		SmartPtr<IProcessor> pipeline = constructor.GetPipeline();
		if (pipeline)
		{
			pipeline->Input(L"Input", nullptr);
		}
	}
	catch (ConstructError& e)
	{
		wcout << e.GetErrorMessage() << std::endl;
	}
}

int main()
{
	time_t start = clock();
	YapDebugger debugger;
//	debugger.DebugPlugin(L"BasicRecon.dll");
//	debugger.DebugPlugin(L"GrappaRecon.dll");
//	debugger.DebugPlugin(L"CompressedSensing.dll");
//	debugger.DebugPlugin(L"BasicRecon_GPU.dll");

//	 ConstructorTest();

	PipelineCompiler compiler;
	auto pipeline = compiler.CompileFile(L"niumag_recon_yap.pipeline");
	if (pipeline)
	{
		pipeline->Input(L"Input", nullptr);
	}

	VdfParser parser;
	auto variable_manager = parser.CompileFile(L"sysParams_yap.txt");

	time_t end = clock();
	printf("the running time is : %f\n", float(end - start) / CLOCKS_PER_SEC);
	return 0;
}

