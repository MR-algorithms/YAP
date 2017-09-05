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

BOOST_AUTO_TEST_CASE(pipeline_compiler_test)
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
