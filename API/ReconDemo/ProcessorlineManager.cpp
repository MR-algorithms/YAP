#include "ProcessorlineManager.h"
#include <cassert>
#include "StringHelper.h"
#include "Implement/DataObject.h"
#include "API/Yap/PipelineCompiler.h"
#include "API/Yap/ProcessorAgent.h"
#include "Implement/CompositeProcessor.h"
#include "API/Yap/PipelineConstructor.h"
#include "Utilities\commonmethod.h"
#include "QtUtilities/FormatString.h"
#include <algorithm>
#include <qmessagebox>
#include <io.h>
#include <direct.h>
#include <fstream>
#include <QDateTime>

#include <QFileInfo>

#include "Processors/Rawdata.h"
#include "Client/DataHelper.h"

#include <QEvent>
#include <QApplication>


using namespace std;
using namespace Yap;
using namespace Databin;

ProcessorlineManager ProcessorlineManager::s_instance;

ProcessorlineManager::ProcessorlineManager()
{

}


ProcessorlineManager& ProcessorlineManager::GetHandle()
{
    return s_instance;

}

ProcessorlineManager::~ProcessorlineManager()
{

}

bool ProcessorlineManager::Pipeline1DforNewScan()
{
    if(_rt_pipeline1D)
        return true;
    try
    {
        Yap::PipelineConstructor constructor;
        constructor.Reset(true);
        constructor.LoadModule(L"BasicRecon.dll");

        constructor.CreateProcessor(L"Display1D", L"Plot1D");
        constructor.MapInput(L"Input", L"Plot1D", L"Input");

        _rt_pipeline1D = constructor.GetPipeline();
        if (_rt_pipeline1D)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    catch (ConstructError& e)
    {
        //wcout << e.GetErrorMessage() << std::endl;
    }

    return true;
}

bool ProcessorlineManager::Pipeline2DforNewScan()
{
    _rt_pipeline2D = this->CreatePipeline(QString("config//pipelines//realtime_recon.pipeline"));

    if(_rt_pipeline2D)
    {
        return true;
    }
    else
    {
        return false;
    }

}


bool ProcessorlineManager::LoadData(const QString& file_path)
{
    auto extension = make_lower(file_path.toStdWString().substr(file_path.toStdWString().size() - 4, 4));

    if (extension == wstring(L".img"))
    {
        // Load img data.
        //if (!LoadImage(file_path.toStdWString()))
            return false;
    }
    else if (extension == L".fid")
    {
        QFileInfo file_info(file_path);
        auto path = file_info.path();

        if (!ProcessFidfile(file_path, QString("config//pipelines//niumag_recon.pipeline"))){
            return false;
        }
    }
    else
    {
        auto extension2 = make_lower(file_path.toStdWString().substr(file_path.toStdWString().size() - 7, 7));
        if (extension2 == wstring(L".niuimg"))
        {
            //Load NiuMriImage
            //if (!LoadNiuImage(file_path.toStdWString()))
                return false;
        }
    }

    return true;
}

void ProcessorlineManager::RunPipeline(const QString& pipeline)
{

    auto result = this->CreatePipeline(pipeline);


    result->Input(L"Input", nullptr);

}


bool ProcessorlineManager::ProcessFidfile(const QString &file_path, const QString &pipelineFile)
{

   Yap::PipelineCompiler compiler;
   auto pipeline = compiler.CompileFile(pipelineFile.toStdWString().c_str());

    QFileInfo info(pipelineFile);
    if (!pipeline)
    {
        QMessageBox::critical(nullptr, QString("Error"), QString("Error compliling the pipeline: ") + info.baseName());
        return false;
    }

    auto reader = pipeline->Find(L"reader");
    if (reader == nullptr)
        return false;

    //auto variables = ScanManager::GetHandle().GetVariables()->Variables();
    //assert(variables);
    //pipeline->SetGlobalVariables(variables);

    auto reader_agent = ProcessorAgent(reader);
    reader_agent.SetString(L"DataPath", file_path.toStdWString().c_str());

    return pipeline->Input(L"Input", nullptr);


}


Yap::SmartPtr<IProcessor> ProcessorlineManager::CreatePipeline(const QString& pipelineFile)
{


    Yap::PipelineCompiler compiler;
    auto pipeline = compiler.CompileFile(pipelineFile.toStdWString().c_str());

    QFileInfo info(pipelineFile);
    if (!pipeline)
    {
        //QMessageBox::critical(nullptr, QString("Error"), QString("Error compliling the pipeline: ") + info.baseName());
        return Yap::SmartPtr<IProcessor>();
    }

     return pipeline;
}

bool ProcessorlineManager::Demo1D()
{

    try
    {
        Yap::PipelineConstructor constructor;
        constructor.Reset(true);
        //constructor.LoadModule(L"BasicRecon.dll");

        constructor.CreateProcessor(L"Display1D", L"Plot1D");

        constructor.MapInput(L"Input", L"Plot1D", L"Input");

        SmartPtr<IProcessor> pipeline = constructor.GetPipeline();
        if (pipeline)
        {

            auto output_data = CreateDemoData1D();
            pipeline->Input(L"Input", output_data.get());
        }
    }
    catch (ConstructError& e)
    {
        //wcout << e.GetErrorMessage() << std::endl;
    }

    return true;
}

bool ProcessorlineManager::Demo2D()
{

    try
    {
        Yap::PipelineConstructor constructor;
        constructor.Reset(true);
        constructor.LoadModule(L"BasicRecon.dll");

        constructor.CreateProcessor(L"Display2D", L"display2D");


        constructor.MapInput(L"Input", L"display2D", L"Input");

        SmartPtr<IProcessor> pipeline = constructor.GetPipeline();
        if (pipeline)
        {

            int width, height, slice_count;
            auto output_data = CreateDemoData2D(width, height, slice_count);
            AddVariables(output_data.get(), height, 0, 1, 1, 0);

            pipeline->Input(L"Input", output_data.get());
        }
    }
    catch (ConstructError& e)
    {
        //wcout << e.GetErrorMessage() << std::endl;
    }

    return true;
}


void ProcessorlineManager::AddVariables(Yap::IData* data,
                  int phase_count,
                  int slice_index,
                  int slice_count,
                  int channel_switch,
                  int channel_index
                  )
{
    DataHelper helper(data);
    if (nullptr == data->GetVariables())    {
        VariableSpace variable;
        //
        if (helper.GetDataType() == DataTypeComplexFloat)   {
            dynamic_cast<Yap::DataObject<std::complex<float>>*>(data)->SetVariables(variable.Variables());
        }
        else {
            dynamic_cast<Yap::DataObject<unsigned short>*>(data)->SetVariables(variable.Variables());
        }
    }

    VariableSpace variables(data->GetVariables());

    variables.AddVariable(L"int",  L"channel_switch", L"channel switch.");
    variables.AddVariable(L"int",  L"channel_index", L"channel index.");
    variables.AddVariable(L"int",  L"slice_count", L"slice count.");
    variables.AddVariable(L"int",  L"slice_index", L"slice index.");
    variables.AddVariable(L"int",  L"phase_count", L"phase count.");

    variables.Set(L"channel_switch", channel_switch);
    variables.Set(L"channel_index", channel_index);
    variables.Set(L"slice_count", slice_count);
    variables.Set(L"slice_index", slice_index);
    variables.Set(L"phase_count", phase_count);


}


bool ProcessorlineManager::OnScanStart(const ScanDimension& scan_dim)
{

    DebugInfo::Output(L"ProcessorlineManager::OnScanStart()", L"Enter...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type2);
    Pipeline1DforNewScan();
    Pipeline2DforNewScan();
    assert(_rt_pipeline1D);
    assert(_rt_pipeline2D);
    //

    Yap::VariableSpace variables;
    variables.AddVariable(L"int", L"scan_signal", L"Scan Signal.");
    variables.AddVariable(L"int",  L"scan_id", L"scan id.");
    variables.AddVariable(L"int",  L"channel_switch", L"channel switch.");
    variables.AddVariable(L"int",  L"freq_count", L"frequcency count.");
    variables.AddVariable(L"int",  L"phase_count", L"phase count.");
    variables.AddVariable(L"int",  L"slice_count", L"slice count.");

    variables.Set(L"scan_signal", static_cast<int>(SSScanStart));
    variables.Set(L"scan_id", scan_dim.scan_id);
    variables.Set(L"channel_switch", scan_dim.channel_switch);
    variables.Set(L"freq_count", scan_dim.freq_count);
    variables.Set(L"phase_count", scan_dim.phase_count);
    variables.Set(L"slice_count", scan_dim.slice_count);

    DebugInfo::Output(L"ProcessorlineManager::OnScanStart()", L"feed ScanStart signal...",
                      reinterpret_cast<int>(this), false, DebugInfo::flow_type2);
    auto output = DataObject<int>::CreateVariableObject(variables.Variables(), nullptr);

    _rt_pipeline2D->Input(L"Input", output.get());

    return true;
}


bool ProcessorlineManager::OnChannelPhasestep(const ScanDimension& scan_dim, shared_ptr<complex<float>> buffer_pointer)
{
    DebugInfo::Output(L"ProcessorlineManager::OnChannelPhasestep()", L"Enter...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type2);
    InputPipeline1D(scan_dim, buffer_pointer);
    InputPipeline2D(scan_dim, buffer_pointer);

    return true;
}
bool ProcessorlineManager::OnScanFinished(const ScanDimension& scan_data)
{

    Yap::VariableSpace variables;
    variables.AddVariable(L"int", L"scan_signal", L"Scan Signal.");
    variables.AddVariable(L"int",  L"scan_id", L"scan id.");

    variables.Set(L"scan_signal", static_cast<int>(SSScanFinished));
    variables.Set(L"scan_id", scan_data.scan_id);

    auto output = DataObject<int>::CreateVariableObject(variables.Variables(), nullptr);

    assert(_rt_pipeline2D);
    _rt_pipeline2D->Input(L"Input", output.get());
    return true;
}

bool ProcessorlineManager::InputPipeline1D(const ScanDimension& scan_dim, shared_ptr<complex<float>> buffer_pointer)
{

    const double PI = 3.1415926;
    static double phi0 = 0;
    phi0 += 0.1*PI;

    DebugInfo::Output(L"ProcessorlineManager::InputPipeline1D()", L"Enter ...",
                      reinterpret_cast<int>(this), true, DebugInfo::flow_type2);
    //auto output_data = CreateDemoData1D(scan_dim.phase_index, phi0);
    auto output_data = CreateData1D(scan_dim, buffer_pointer);
    assert(_rt_pipeline1D);
    _rt_pipeline1D->Input(L"Input", output_data.get());
    return true;

}

bool ProcessorlineManager::InputPipeline2D(const ScanDimension& scan_dim, shared_ptr<complex<float>> buffer_pointer)
{

    auto output = CreateData1D(scan_dim, buffer_pointer);
    assert(_rt_pipeline2D);
    _rt_pipeline2D->Input(L"Input", output.get());
    return true;

}

Yap::SmartPtr<Yap::IData> ProcessorlineManager::CreateDemoData1D(int phase_index, double phi0)
{

    const double PI = 3.1415926;
    int data_vector2_size = 314;
    std::complex<float> * data_vector2 = new std::complex<float>[data_vector2_size];
    double dw = 4*PI / data_vector2_size;
    for(int i = 0; i < data_vector2_size; i ++)
    {

      double temp = phase_index;//data_vector2_size + 10 - i;
      data_vector2[i].real( temp* 10* sin(i * dw + phi0) );
      data_vector2[i].imag( temp* 10* cos(i * dw + phi0) );
    }

    //
    Yap::Dimensions dimensions;
    dimensions(Yap::DimensionReadout, 0U, data_vector2_size)
        (Yap::DimensionPhaseEncoding, 0U, 1)
        (Yap::DimensionSlice, 0U, 1);

    auto output_data1 = Yap::YapShared(
                new Yap::DataObject<std::complex<float>>(nullptr, data_vector2, dimensions, nullptr, nullptr));


    return output_data1;

}
Yap::SmartPtr<Yap::IData> ProcessorlineManager::CreateDemoData2D(int &width, int &height, int &slice_count)
{
    Yap::Dimensions dimensions;
    width = 512;
    height = 512;
    slice_count = 1;

    auto complex_slices = std::shared_ptr<std::complex<float>>(new std::complex<float>[width * height * slice_count]);

    auto ushort_slices = new unsigned short[width * height * slice_count];

    for(unsigned i = 0; i < slice_count; i ++)
    {
        unsigned slice_pixels = width * height;

        for(unsigned j = 0; j < height; j ++)
        {
            for(unsigned k = 0; k < width; k ++)
            {

                unsigned index = i * slice_pixels + j* width + k;

                if(j < height/2)
                {
                    complex_slices.get()[index].real(0);
                    complex_slices.get()[index].imag(0);


                }
                else
                {
                    complex_slices.get()[index].real(10* (1+i));
                    complex_slices.get()[index].imag(10* (1+i));
                }

                ushort_slices[index] = j;

            }
        }

    }
    //std::complex<float> test[512*2];
    //memcpy(test, complex_slices.get() + 255*512, sizeof(std::complex<float>)* 512*2);

    dimensions(Yap::DimensionReadout, 0U, width)
        (Yap::DimensionPhaseEncoding, 0U, height)
        (Yap::DimensionSlice, 0U, slice_count);

    auto output_data1 = Yap::YapShared(
                new Yap::DataObject<unsigned short>(nullptr, ushort_slices, dimensions, nullptr, nullptr));


    return output_data1;


}


Yap::SmartPtr<Yap::IData> ProcessorlineManager::CreateData1D(const ScanDimension& scan_dim, shared_ptr<complex<float>> buffer_pointer)
{

    Yap::Dimensions dimensions;
    dimensions(Yap::DimensionReadout, 0U, scan_dim.freq_count)
        (Yap::DimensionPhaseEncoding, scan_dim.phase_index, 1)//
        (Yap::DimensionSlice, scan_dim.slice_index, 1)
        (Yap::DimensionChannel, scan_dim.channel_index, 1);

    complex<float>* data_buffer = new complex<float>[scan_dim.freq_count];
	memcpy(data_buffer, buffer_pointer.get(), scan_dim.freq_count * sizeof(complex<float>));

    auto output_data = Yap::YapShared(
                new Yap::DataObject<std::complex<float>>(nullptr, data_buffer, dimensions, nullptr, nullptr));

    if (output_data->GetVariables() == nullptr)
    {
        Yap::VariableSpace variables;
        output_data->SetVariables(variables.Variables());

        variables.AddVariable(L"int", L"scan_signal", L"scan signal.");
		variables.AddVariable(L"int", L"scan_id", L"scan id.");
		variables.AddVariable(L"int", L"channel_switch", L"channel switch.");

        variables.Set(L"scan_signal", static_cast<int>(SSChannelPhaseStep));
		variables.Set(L"scan_id", scan_dim.scan_id);
		variables.Set(L"channel_switch", scan_dim.channel_switch);
    }
    return output_data;

}

