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
#include <log4cplus/initializer.h>
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

	PipelineCompiler compiler;
	auto pipeline = compiler.CompileFile(L"Pipelines\\ImgPythonShow.pipeline");
	if (pipeline.get() == nullptr)
	{
		return;
	}
	pipeline->SetGlobalVariables(variable_manager->Variables());

	if (pipeline) 
	{
		pipeline->Input(L"Input", nullptr);
	}

	auto sfo1 = variable_manager->Get<double>(L"SFO1");

	ModuleManager::GetInstance().Release();
}

bool FFT3DTest()
{
	PipelineCompiler compiler;
	auto pipeline = compiler.CompileFile(L"Pipelines\\niumag_recon_yap.pipeline");
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

bool VdfParserTest()
{
	VdfParser parser;
	auto variable_manager = parser.CompileFile(L"sysParams_yap.txt");

	variable_manager->Set<double>(L"Rotation.GReadX", 7.77);
	auto v1 = variable_manager->Get<double>(L"Rotation.GReadX");

	variable_manager->Set<double>(L"Gx.A1", 1.00);
	auto v2 = variable_manager->Get<double>(L"Gx.A1");

	variable_manager->Set<bool>(L"Form", false);
	auto v3 = variable_manager->Get<bool>(L"Form");

	variable_manager->Set<int>(L"TD", 256);
	auto v4 = variable_manager->Get<int>(L"TD");

	variable_manager->Set<const wchar_t* const>(L"Sequence", L"FID1");
	auto v5 = variable_manager->Get<const wchar_t * const>(L"Sequence");

	variable_manager->ResizeArray(L"VDL1", 5);
	variable_manager->Set<double>(L"VDL1[0]", 1.0);
	variable_manager->Set<double>(L"VDL1[1]", 2.0);
	auto v6 = variable_manager->Get<double>(L"VDL1[1]");
	auto vdl1 = variable_manager->GetArrayWithSize<double>(L"VDL1");

	variable_manager->ResizeArray(L"GradMatrixList", 7);
	variable_manager->Set<double>(L"GradMatrixList[1].GReadX", 1.00);
	variable_manager->Set<double>(L"GradMatrixList[2].GReadX", 2.00);
	auto v7 = variable_manager->Get<double>(L"GradMatrixList[1].GReadX");
	auto gml = variable_manager->GetArrayWithSize<double>(L"GradMatrixList");

	// 错误的参数输入测试
//	variable_manager->Set<int>(L"abc", 4); // 无此变量名
//	auto v = variable_manager->Get<int>(L"abc"); // 无此变量名
//	variable_manager->Set<double>(L"TD", 6); // 名字和类型不匹配
//	variable_manager->Set<double>(L"VDL1[6]", 6.0); // 数组越界
//	auto v = variable_manager->Get<double>(L"VDL1[6]"); // 数组越界

	return variable_manager.get() != nullptr;
}

int main()
{
	log4cplus::Initializer initializer;

	time_t start = clock();

//	ConstructorTest();
//  PipelineTest();
//	VdfParserTest();
	FFT3DTest();

	time_t end = clock();
	printf("\n");
	printf("the running time is : %f\n", float(end - start) / CLOCKS_PER_SEC);
	system("pause");

	return 0;
}

