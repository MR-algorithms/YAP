// PipelineUnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/test/unit_test.hpp>

#include "Yap/PipelineCompiler.h"

using namespace Yap;

Yap::SmartPtr<CompositeProcessor> GetPipeline(const wchar_t * script)
{
	PipelineCompiler compiler;
	return compiler.Compile(script);
}

void CheckProcessor(Yap::IProcessor* processor)
{
	BOOST_CHECK(processor != nullptr);
	BOOST_CHECK(processor->Inputs() != nullptr);
	BOOST_CHECK(processor->Inputs()->Find(L"Input") != nullptr);
}

BOOST_AUTO_TEST_CASE(processors_in_pipeline)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"CalcuArea calcu_area;"
		L"ChannelDataCollector channel_data_collector;"
		L"ChannelIterator channel_iterator;"
		L"ChannelMerger channel_merger;"
		L"CmrDataReader cmr_data_reader;"
		L"ComplexSplitter complex_splitter;"
		L"DataTypeConvertor data_type_convertor;"
		L"DcRemover dc_remover;"
		L"Difference difference;"
		L"Fft1D fft1d;"
		L"Fft2D fft2d;"
		L"FineCF fine_cf;"
		L"JpegExporter jpeg_exporter;"
		L"ModulePhase module_phase;"
		L"NiumagFidReader niumag_fid_reader;"
		L"NiumagFidWriter niumag_fid_writer;"
		L"NiumagImgReader niumag_img_reader;"
		L"NiuMriImageReader niumri_image_reader;"
		L"NiuMriImageWriter niumri_image_writer;"
		L"NLM nlm;"
		L"Nlmeans nlmeans;"
		L"SamplingMaskCreator sampling_mask_creator;"
		L"SliceIterator slice_iterator;"
		L"SliceMerger slice_merger;"
		L"SliceSelector slice_selector;"
		L"SubSampling sub_sampling;"
		L"ZeroFilling zero_filling;"
	);

	auto processor = pipe->Find(L"calcu_area");
	CheckProcessor(processor);

	processor = pipe->Find(L"channel_data_collector");
	CheckProcessor(processor);

	processor = pipe->Find(L"channel_iterator");
	CheckProcessor(processor);

	processor = pipe->Find(L"channel_merger");
	CheckProcessor(processor);

	processor = pipe->Find(L"cmr_data_reader");
	CheckProcessor(processor);

	processor = pipe->Find(L"complex_splitter");
	CheckProcessor(processor);

	processor = pipe->Find(L"data_type_convertor");
	CheckProcessor(processor);

	processor = pipe->Find(L"dc_remover");
	CheckProcessor(processor);

	processor = pipe->Find(L"difference");
	CheckProcessor(processor);

	processor = pipe->Find(L"fft1d");
	CheckProcessor(processor);

	processor = pipe->Find(L"fft2d");
	CheckProcessor(processor);

	processor = pipe->Find(L"fine_cf");
	CheckProcessor(processor);

	processor = pipe->Find(L"jpeg_exporter");
	CheckProcessor(processor);

	processor = pipe->Find(L"module_phase");
	CheckProcessor(processor);

	processor = pipe->Find(L"niumag_fid_reader");
	CheckProcessor(processor);

	processor = pipe->Find(L"niumag_fid_writer");
	CheckProcessor(processor);

	processor = pipe->Find(L"niumag_img_reader");
	CheckProcessor(processor);

	processor = pipe->Find(L"niumri_image_reader");
	CheckProcessor(processor);

	processor = pipe->Find(L"niumri_image_writer");
	CheckProcessor(processor);

	processor = pipe->Find(L"nlm");
	CheckProcessor(processor);

	processor = pipe->Find(L"nlmeans");
	CheckProcessor(processor);

	processor = pipe->Find(L"sampling_mask_creator");
	CheckProcessor(processor);

	processor = pipe->Find(L"slice_iterator");
	CheckProcessor(processor);

	processor = pipe->Find(L"slice_merger");
	CheckProcessor(processor);

	processor = pipe->Find(L"slice_selector");
	CheckProcessor(processor);

	processor = pipe->Find(L"sub_sampling");
	CheckProcessor(processor);

	processor = pipe->Find(L"zero_filling");
	CheckProcessor(processor);
}
BOOST_AUTO_TEST_CASE(processor_functions)
{
	VariableSpace variables;
	variables.Add(L"bool", L"var1", L"var1.");
	//variables.Add(L"float", L"var2", L"var2.");
	variables.Set(L"var1", true);
	//variables.Set(L"var2", 3.14f);
	
	auto data = DataObject<int>::CreateVariableObject(variables.Variables(), nullptr);
	
	BOOST_CHECK(data->GetVariables() != nullptr);
	
	VariableSpace variables2(data->GetVariables());
	bool var1 = variables2.Get<bool>(L"var1");
	//float var2 = variables2.Get<float>(L"var2");
	
	BOOST_CHECK(var1 == true);
	//BOOST_CHECK(var2 == 3.14f);

}

BOOST_AUTO_TEST_CASE(processor_CalcuArea)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"CalcuArea calcu_area;");

	auto processor = pipe->Find(L"calcu_area");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeDouble);
	BOOST_CHECK(input_port->GetDimensionCount() == 1);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto area = properties->Find(L"Area");
	BOOST_CHECK(area != nullptr);
	BOOST_CHECK(area->GetType() == VariableFloat);
}

BOOST_AUTO_TEST_CASE(processor_ChannelDataCollector)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"ChannelDataCollector channel_data_collector;");

	auto processor = pipe->Find(L"channel_data_collector");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(input_port->GetDimensionCount() == 2);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == 3);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto cc = properties->Find(L"ChannelCount");
	BOOST_CHECK(cc != nullptr);
	BOOST_CHECK(cc->GetType() == VariableInt);
}

BOOST_AUTO_TEST_CASE(processor_ChannelIterator)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"ChannelIterator channel_iterator;");

	auto processor = pipe->Find(L"channel_iterator");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == 2);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto slice_index = properties->Find(L"SliceIndex");
	BOOST_CHECK(slice_index != nullptr);
	BOOST_CHECK(slice_index->GetType() == VariableInt);
}

BOOST_AUTO_TEST_CASE(processor_ChannelMerger)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"ChannelMerger channel_merger;");

	auto processor = pipe->Find(L"channel_merger");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeFloat);
	BOOST_CHECK(input_port->GetDimensionCount() == 2);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == 2);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto channel_count = properties->Find(L"ChannelCount");
	BOOST_CHECK(channel_count != nullptr);
	BOOST_CHECK(channel_count->GetType() == VariableInt);
	auto channel_switch = properties->Find(L"ChannelSwitch");
	BOOST_CHECK(channel_switch != nullptr);
	BOOST_CHECK(channel_switch->GetType() == VariableInt);
}

BOOST_AUTO_TEST_CASE(processor_CmrDataReader)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"CmrDataReader cmr_data_reader;");

	auto processor = pipe->Find(L"cmr_data_reader");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeUnknown);
	BOOST_CHECK(input_port->GetDimensionCount() == 0);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto data_path = properties->Find(L"DataPath");
	BOOST_CHECK(data_path != nullptr);
	BOOST_CHECK(data_path->GetType() == VariableString);
	auto channel_count = properties->Find(L"ChannelCount");
	BOOST_CHECK(channel_count != nullptr);
	BOOST_CHECK(channel_count->GetType() == VariableInt);
	auto channel_switch = properties->Find(L"ChannelSwitch");
	BOOST_CHECK(channel_switch != nullptr);
	BOOST_CHECK(channel_switch->GetType() == VariableInt);
	auto GroupCount = properties->Find(L"GroupCount");
	BOOST_CHECK(GroupCount != nullptr);
	BOOST_CHECK(GroupCount->GetType() == VariableInt);
}

BOOST_AUTO_TEST_CASE(processor_ComplexSplitter)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"ComplexSplitter complex_splitter;");

	auto processor = pipe->Find(L"complex_splitter");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeComplexDouble);
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto real_port = outputs->Find(L"Real");
	BOOST_CHECK(real_port != nullptr);
	BOOST_CHECK(real_port->GetDataType() == DataTypeDouble);
	BOOST_CHECK(real_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto imaginary_port = outputs->Find(L"Imaginary");
	BOOST_CHECK(imaginary_port != nullptr);
	BOOST_CHECK(imaginary_port->GetDataType() == DataTypeDouble);
	BOOST_CHECK(imaginary_port->GetDimensionCount() == YAP_ANY_DIMENSION);
}

BOOST_AUTO_TEST_CASE(processor_DataTypeConvertor)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"DataTypeConvertor data_type_convertor;");

	auto processor = pipe->Find(L"data_type_convertor");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeAll);
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto bool_port = outputs->Find(L"Bool");
	BOOST_CHECK(bool_port != nullptr);
	BOOST_CHECK(bool_port->GetDataType() == DataTypeBool);
	BOOST_CHECK(bool_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto char_port = outputs->Find(L"Char");
	BOOST_CHECK(char_port != nullptr);
	BOOST_CHECK(char_port->GetDataType() == DataTypeUnsignedChar);
	BOOST_CHECK(char_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto short_port = outputs->Find(L"Short");
	BOOST_CHECK(short_port != nullptr);
	BOOST_CHECK(short_port->GetDataType() == DataTypeShort);
	BOOST_CHECK(short_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto unsignedshort_port = outputs->Find(L"UnsignedShort");
	BOOST_CHECK(unsignedshort_port != nullptr);
	BOOST_CHECK(unsignedshort_port->GetDataType() == DataTypeUnsignedShort);
	BOOST_CHECK(unsignedshort_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto int_port = outputs->Find(L"Int");
	BOOST_CHECK(int_port != nullptr);
	BOOST_CHECK(int_port->GetDataType() == DataTypeInt);
	BOOST_CHECK(int_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto insignedint_port = outputs->Find(L"UnsignedInt");
	BOOST_CHECK(insignedint_port != nullptr);
	BOOST_CHECK(insignedint_port->GetDataType() == DataTypeUnsignedInt);
	BOOST_CHECK(insignedint_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto float_port = outputs->Find(L"Float");
	BOOST_CHECK(float_port != nullptr);
	BOOST_CHECK(float_port->GetDataType() == DataTypeFloat);
	BOOST_CHECK(float_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto double_port = outputs->Find(L"Double");
	BOOST_CHECK(double_port != nullptr);
	BOOST_CHECK(double_port->GetDataType() == DataTypeDouble);
	BOOST_CHECK(double_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto complexfloat_port = outputs->Find(L"ComplexFloat");
	BOOST_CHECK(complexfloat_port != nullptr);
	BOOST_CHECK(complexfloat_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(complexfloat_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto complexdouble_port = outputs->Find(L"ComplexDouble");
	BOOST_CHECK(complexdouble_port != nullptr);
	BOOST_CHECK(complexdouble_port->GetDataType() == DataTypeComplexDouble);
	BOOST_CHECK(complexdouble_port->GetDimensionCount() == YAP_ANY_DIMENSION);
}

BOOST_AUTO_TEST_CASE(processor_DcRemover)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"DcRemover dc_remover;");

	auto processor = pipe->Find(L"dc_remover");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == (DataTypeComplexFloat | DataTypeComplexDouble));
	BOOST_CHECK(input_port->GetDimensionCount() == 2);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == (DataTypeComplexFloat | DataTypeComplexDouble));
	BOOST_CHECK(output_port->GetDimensionCount() == 2);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto inplace = properties->Find(L"Inplace");
	BOOST_CHECK(inplace != nullptr);
	BOOST_CHECK(inplace->GetType() == VariableBool);
	auto corner_size = properties->Find(L"CornerSize");
	BOOST_CHECK(corner_size != nullptr);
	BOOST_CHECK(corner_size->GetType() == VariableInt);
}

BOOST_AUTO_TEST_CASE(processor_Difference)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"Difference difference;");

	auto processor = pipe->Find(L"difference");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeAll);
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto reference_port = inputs->Find(L"Reference");
	BOOST_CHECK(reference_port != nullptr);
	BOOST_CHECK(reference_port->GetDataType() == DataTypeAll);
	BOOST_CHECK(reference_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeAll);
	BOOST_CHECK(output_port->GetDimensionCount() == YAP_ANY_DIMENSION);
}

BOOST_AUTO_TEST_CASE(processor_Fft1D)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"Fft1D fft1d;");

	auto processor = pipe->Find(L"fft1d");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == (DataTypeComplexFloat | DataTypeComplexDouble));
	BOOST_CHECK(input_port->GetDimensionCount() == 1);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == (DataTypeComplexFloat | DataTypeComplexDouble));
	BOOST_CHECK(output_port->GetDimensionCount() == 1);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto in_place = properties->Find(L"InPlace");
	BOOST_CHECK(in_place != nullptr);
	BOOST_CHECK(in_place->GetType() == VariableBool);
	auto inverse = properties->Find(L"Inverse");
	BOOST_CHECK(inverse != nullptr);
	BOOST_CHECK(inverse->GetType() == VariableBool);
}

BOOST_AUTO_TEST_CASE(processor_Fft2D)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"Fft2D fft2d;");

	auto processor = pipe->Find(L"fft2d");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(input_port->GetDimensionCount() == 2);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == 2);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto in_place = properties->Find(L"InPlace");
	BOOST_CHECK(in_place != nullptr);
	BOOST_CHECK(in_place->GetType() == VariableBool);
	auto inverse = properties->Find(L"Inverse");
	BOOST_CHECK(inverse != nullptr);
	BOOST_CHECK(inverse->GetType() == VariableBool);
}

BOOST_AUTO_TEST_CASE(processor_FineCF)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"FineCF fine_cf;");

	auto processor = pipe->Find(L"fine_cf");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == (DataTypeFloat | DataTypeDouble));
	BOOST_CHECK(input_port->GetDimensionCount() == 1);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto o1 = properties->Find(L"O1");
	BOOST_CHECK(o1 != nullptr);
	BOOST_CHECK(o1->GetType() == VariableFloat);
	auto sw = properties->Find(L"SW");
	BOOST_CHECK(sw != nullptr);
	BOOST_CHECK(sw->GetType() == VariableFloat);
	auto reverse_axis = properties->Find(L"ReverseAxis");
	BOOST_CHECK(reverse_axis != nullptr);
	BOOST_CHECK(reverse_axis->GetType() == VariableBool);
	auto cf = properties->Find(L"CF");
	BOOST_CHECK(cf != nullptr);
	BOOST_CHECK(cf->GetType() == VariableFloat);
}

BOOST_AUTO_TEST_CASE(processor_JpegExporter)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"JpegExporter jpeg_exporter;");

	auto processor = pipe->Find(L"jpeg_exporter");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK((input_port->GetDataType() & DataTypeFloat) == DataTypeFloat);
	BOOST_CHECK((input_port->GetDataType() & DataTypeUnsignedShort) == DataTypeUnsignedShort);
	BOOST_CHECK((input_port->GetDataType() & DataTypeShort) == DataTypeShort);
	BOOST_CHECK(input_port->GetDimensionCount() == 2);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto export_folder = properties->Find(L"ExportFolder");
	BOOST_CHECK(export_folder != nullptr);
	BOOST_CHECK(export_folder->GetType() == VariableString);
}

BOOST_AUTO_TEST_CASE(processor_ModulePhase)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"ModulePhase module_phase;");

	auto processor = pipe->Find(L"module_phase");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == (DataTypeComplexFloat | DataTypeComplexDouble));
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto module_port = outputs->Find(L"Module");
	BOOST_CHECK(module_port != nullptr);
	BOOST_CHECK(module_port->GetDataType() == (DataTypeFloat | DataTypeDouble));
	BOOST_CHECK(module_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto phase_port = outputs->Find(L"Phase");
	BOOST_CHECK(phase_port != nullptr);
	BOOST_CHECK(phase_port->GetDataType() == (DataTypeFloat | DataTypeDouble));
	BOOST_CHECK(phase_port->GetDimensionCount() == YAP_ANY_DIMENSION);
}

BOOST_AUTO_TEST_CASE(processor_NiumagFidReader)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"NiumagFidReader niumag_fid_reader;");

	auto processor = pipe->Find(L"niumag_fid_reader");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeUnknown);
	BOOST_CHECK(input_port->GetDimensionCount() == 0);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto data_path = properties->Find(L"DataPath");
	BOOST_CHECK(data_path != nullptr);
	BOOST_CHECK(data_path->GetType() == VariableString);
}

BOOST_AUTO_TEST_CASE(processor_NiumagFidWriter)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"NiumagFidWriter niumag_fid_writer;");

	auto processor = pipe->Find(L"niumag_fid_writer");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto export_folder = properties->Find(L"ExportFolder");
	BOOST_CHECK(export_folder != nullptr);
	BOOST_CHECK(export_folder->GetType() == VariableString);
	auto file_name = properties->Find(L"FileName");
	BOOST_CHECK(file_name != nullptr);
	BOOST_CHECK(file_name->GetType() == VariableString);
	auto save_path = properties->Find(L"SavePath");
	BOOST_CHECK(save_path != nullptr);
	BOOST_CHECK(save_path->GetType() == VariableString);
}

BOOST_AUTO_TEST_CASE(processor_NiumagImgReader)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"NiumagImgReader niumag_img_reader;");

	auto processor = pipe->Find(L"niumag_img_reader");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeUnknown);
	BOOST_CHECK(input_port->GetDimensionCount() == 0);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeUnsignedShort);
	BOOST_CHECK(output_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto data_path = properties->Find(L"DataPath");
	BOOST_CHECK(data_path != nullptr);
	BOOST_CHECK(data_path->GetType() == VariableString);
}

BOOST_AUTO_TEST_CASE(processor_NiuMriImageReader)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"NiuMriImageReader niumri_image_reader;");

	auto processor = pipe->Find(L"niumri_image_reader");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeUnknown);
	BOOST_CHECK(input_port->GetDimensionCount() == 0);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeUnsignedShort);
	BOOST_CHECK(output_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto data_path = properties->Find(L"DataPath");
	BOOST_CHECK(data_path != nullptr);
	BOOST_CHECK(data_path->GetType() == VariableString);
}

BOOST_AUTO_TEST_CASE(processor_NiuMriImageWriter)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"NiuMriImageWriter niumri_image_writer;");

	auto processor = pipe->Find(L"niumri_image_writer");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeUnsignedShort);
	BOOST_CHECK(input_port->GetDimensionCount() == 3);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto export_folder = properties->Find(L"ExportFolder");
	BOOST_CHECK(export_folder != nullptr);
	BOOST_CHECK(export_folder->GetType() == VariableString);
	auto file_name = properties->Find(L"FileName");
	BOOST_CHECK(file_name != nullptr);
	BOOST_CHECK(file_name->GetType() == VariableString);
}

BOOST_AUTO_TEST_CASE(processor_NLM)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"NLM nlm;");

	auto processor = pipe->Find(L"nlm");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeFloat);
	BOOST_CHECK(input_port->GetDimensionCount() == 2);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == 2);
}

BOOST_AUTO_TEST_CASE(processor_Nlmeans)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"Nlmeans nlmeans;");

	auto processor = pipe->Find(L"nlmeans");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeFloat);
	BOOST_CHECK(input_port->GetDimensionCount() == 2);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == 2);
}

BOOST_AUTO_TEST_CASE(processor_SamplingMaskCreator)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"SamplingMaskCreator sampling_mask_creator;");

	auto processor = pipe->Find(L"sampling_mask_creator");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	VariableSpace variables{ properties };
	auto pow = properties->Find(L"Pow");
	BOOST_CHECK(pow != nullptr);
	BOOST_CHECK(pow->GetType() == VariableFloat);
	BOOST_CHECK(variables.Get<double>(L"Pow") == (double)3.0);
	auto sample_percent = properties->Find(L"SamplePercent");
	BOOST_CHECK(sample_percent != nullptr);
	BOOST_CHECK(sample_percent->GetType() == VariableFloat);
	BOOST_CHECK(variables.Get<double>(L"SamplePercent") == (double)0.4);
	auto radius = properties->Find(L"Radius");
	BOOST_CHECK(radius != nullptr);
	BOOST_CHECK(radius->GetType() == VariableFloat);
	BOOST_CHECK(variables.Get<double>(L"Radius") == (double)0.2);
	auto equal_subsampling = properties->Find(L"EqualSubsampling");
	BOOST_CHECK(equal_subsampling != nullptr);
	BOOST_CHECK(equal_subsampling->GetType() == VariableBool);
	auto random_subsampling = properties->Find(L"RandomSubsampling");
	BOOST_CHECK(random_subsampling != nullptr);
	BOOST_CHECK(random_subsampling->GetType() == VariableBool);
	auto rate = properties->Find(L"Rate");
	BOOST_CHECK(rate != nullptr);
	BOOST_CHECK(rate->GetType() == VariableInt);
	auto asc_count = properties->Find(L"AcsCount");
	BOOST_CHECK(asc_count != nullptr);
	BOOST_CHECK(asc_count->GetType() == VariableInt);
}

BOOST_AUTO_TEST_CASE(processor_SliceIterator)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"SliceIterator slice_iterator;");

	auto processor = pipe->Find(L"slice_iterator");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == (DataTypeComplexFloat | DataTypeUnsignedShort));
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == (DataTypeComplexFloat | DataTypeUnsignedShort));
	BOOST_CHECK(output_port->GetDimensionCount() == 2);
}

BOOST_AUTO_TEST_CASE(processor_SliceMerger)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"SliceMerger slice_merger;");

	auto processor = pipe->Find(L"slice_merger");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeAll);
	BOOST_CHECK(input_port->GetDimensionCount() == 2);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeAll);
	BOOST_CHECK(output_port->GetDimensionCount() == 3);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto slice_count = properties->Find(L"SliceCount");
	BOOST_CHECK(slice_count != nullptr);
	BOOST_CHECK(slice_count->GetType() == VariableInt);
}

BOOST_AUTO_TEST_CASE(processor_SliceSelector)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"SliceSelector slice_selector;");

	auto processor = pipe->Find(L"slice_selector");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == DataTypeComplexFloat);
	BOOST_CHECK(output_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto slice_index = properties->Find(L"SliceIndex");
	BOOST_CHECK(slice_index != nullptr);
	BOOST_CHECK(slice_index->GetType() == VariableInt);
}

BOOST_AUTO_TEST_CASE(processor_SubSampling)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"SubSampling sub_sampling;");

	auto processor = pipe->Find(L"sub_sampling");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == (DataTypeComplexFloat | DataTypeComplexDouble));
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);
	auto mask_port = inputs->Find(L"Mask");
	BOOST_CHECK(mask_port != nullptr);
	BOOST_CHECK(mask_port->GetDataType() == DataTypeFloat);
	BOOST_CHECK(mask_port->GetDimensionCount() == 2);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == (DataTypeComplexFloat | DataTypeComplexDouble));
	BOOST_CHECK(output_port->GetDimensionCount() == YAP_ANY_DIMENSION);
}

BOOST_AUTO_TEST_CASE(processor_ZeroFilling)
{
	auto pipe = GetPipeline(
		L"import \"BasicRecon.dll\";"
		L"ZeroFilling zero_filling(DestWidth = 1);");

	auto processor = pipe->Find(L"zero_filling");
	BOOST_CHECK(processor != nullptr);

	auto inputs = processor->Inputs();
	BOOST_CHECK(inputs != nullptr);
	auto input_port = inputs->Find(L"Input");
	BOOST_CHECK(input_port != nullptr);
	BOOST_CHECK(input_port->GetDataType() == (DataTypeComplexFloat | DataTypeComplexDouble));
	BOOST_CHECK(input_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto outputs = processor->Outputs();
	BOOST_CHECK(outputs != nullptr);
	auto output_port = outputs->Find(L"Output");
	BOOST_CHECK(output_port != nullptr);
	BOOST_CHECK(output_port->GetDataType() == (DataTypeComplexFloat | DataTypeComplexDouble));
	BOOST_CHECK(output_port->GetDimensionCount() == YAP_ANY_DIMENSION);

	auto properties = processor->GetProperties();
	BOOST_CHECK(properties != nullptr);
	auto dest_width = properties->Find(L"DestWidth");
	BOOST_CHECK(dest_width != nullptr);
	BOOST_CHECK(dest_width->GetType() == VariableInt);
	auto dest_height = properties->Find(L"DestHeight");
	BOOST_CHECK(dest_height != nullptr);
	BOOST_CHECK(dest_height->GetType() == VariableInt);
	auto left = properties->Find(L"Left");
	BOOST_CHECK(left != nullptr);
	BOOST_CHECK(left->GetType() == VariableInt);
	auto top = properties->Find(L"Top");
	BOOST_CHECK(top != nullptr);
	BOOST_CHECK(top->GetType() == VariableInt);
}