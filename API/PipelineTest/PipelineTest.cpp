// PipelineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ProcessorDebugger.h"
#include "Yap/PipelineConstructor.h"
#include "Yap/PipelineCompiler.h"
#include "Implement/CompositeProcessor.h"
#include "Implement/DataObject.h"

#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include "Yap/VdfParser.h"
#include "Implement/LogImpl.h"
#include "Implement/LogUserImpl.h"
#include "Yap/ModuleManager.h"

using namespace std;
using namespace Yap;

void ConstructorTest()
{
	try
	{
		PipelineConstructor constructor;
		constructor.Reset(true);
		constructor.LoadModule(L"BasicRecon.dll");
		constructor.CreateProcessor(L"CmrDataReader", L"reader");
		constructor.SetProperty(L"reader", L"DataPath",
			L"D:\\test_data\\RawData_256\\RawData");
			//L"D:\\test_data\\1.3.6.1.4.1.31636.24815489.151224005\\1");

		constructor.SetProperty(L"reader", L"ChannelCount", L"4");
		constructor.SetProperty(L"reader", L"ChannelSwitch", L"15"); // use all four channels.

		constructor.CreateProcessor(L"SliceIterator", L"slice_iterator");
		constructor.CreateProcessor(L"DcRemover", L"dc_remover");
		constructor.CreateProcessor(L"ZeroFilling", L"zero_filling");
		constructor.CreateProcessor(L"Fft2D", L"fft");
		constructor.CreateProcessor(L"ModulePhase", L"module_phase");

		constructor.CreateProcessor(L"JpegExporter", L"jpeg_exporter");
		constructor.SetProperty(L"jpeg_exporter", L"ExportFolder", L"d:\\output");

		constructor.Link(L"reader", L"slice_iterator");
		constructor.Link(L"slice_iterator", L"dc_remover");
		
		//constructor.Link(L"reader", L"slice_selector");
		//constructor.Link(L"slice_selector", L"dc_remover");
		constructor.Link(L"dc_remover", L"zero_filling");
		constructor.Link(L"zero_filling", L"fft");
		constructor.Link(L"fft", L"module_phase");
		//constructor.Link(L"slice_selector", L"module_phase");

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

void PluginDebugTest()
{
	YapDebugger debugger;
	debugger.DebugPlugin(L"BasicRecon.dll");
//	debugger.DebugPlugin(L"GrappaRecon.dll");
//	debugger.DebugPlugin(L"CompressedSensing.dll");
//	debugger.DebugPlugin(L"BasicRecon_GPU.dll");
}

void PipelineTest()
{
	VdfParser parser;
	auto variable_manager = parser.CompileFile(L"sysParams_yap.txt");

	{
		PipelineCompiler compiler;
		//auto pipeline = compiler.CompileFile(L"Pipelines\\Radiomics_test.pipeline"); // PengJinFa test for push
		auto pipeline = compiler.CompileFile(L"Pipelines\\test1.pipeline");

		if (pipeline.get() == nullptr)
		{
			return;
		}
		pipeline->SetGlobalVariables(variable_manager->Variables());

		if (pipeline)
		{
			pipeline->Input(L"Input", nullptr);
		}
	}

	ModuleManager::GetInstance().Release();
}

bool FFT3DTest()
{
	PipelineCompiler compiler;
	auto pipeline = compiler.CompileFile(L"Pipelines\\FFT3DTest.pipeline");
	if (pipeline.get() == nullptr)
	{
		return false;
	}

	if (pipeline)
	{
		return pipeline->Input(L"Input", nullptr);
	}

	return false;
}

bool PartialFFTTest()
{
	PipelineCompiler compiler;
	auto pipeline = compiler.CompileFile(L"Pipelines\\PartialFFT.pipeline");
	if (pipeline.get() == nullptr)
	{
		return false;
	}

	if (pipeline)
	{
		return pipeline->Input(L"Input", nullptr);
	}

	return false;
}

int main()
{
	auto complex_slices = std::shared_ptr<std::complex<float>>(new std::complex<float>[10]);

	complex_slices.get()[0].imag(1.2f);
	complex_slices.get()[0].real(1.7f);
	
	complex_slices.get()[2].imag(5.2f);
	complex_slices.get()[2].real(5.7f);
	auto test = complex_slices.get();
	std::complex<float> test2[10];

	memcpy(test2, test, sizeof(std::complex<float>) *10);
	
	time_t start = clock();

//	ConstructorTest();
	PipelineTest();
//	FFT3DTest();
//	PartialFFTTest();

	time_t end = clock();
	printf("\n");
	printf("the running time is : %f\n", float(end - start) / CLOCKS_PER_SEC);
	system("pause");

	return 0;
}


