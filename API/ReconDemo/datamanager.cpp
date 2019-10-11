#include "datamanager.h"
#include <cassert>
#include "StringHelper.h"
#include "Implement/DataObject.h"
#include "API/Yap/PipelineCompiler.h"
#include "API/Yap/ProcessorAgent.h"
#include "Implement/CompositeProcessor.h"
#include "API/Yap/PipelineConstructor.h"
#include "commonmethod.h"
#include <algorithm>
#include <qmessagebox>
#include <io.h>
#include <direct.h>
#include <fstream>
#include <QDateTime>

#include <QFileInfo>


#include "Client/DataHelper.h"

#include <QEvent>
#include <QApplication>

using namespace std;
using namespace Yap;

DataManager DataManager::s_instance;

DataManager::DataManager()
{

}


DataManager& DataManager::GetHandle()
{
    return s_instance;

}

DataManager::~DataManager()
{
    //_mythread.join();
}
bool DataManager::ReceiveData(DataPackage &package, int cmd_id)
{
    switch(cmd_id)
    {
    case SAMPLE_DATA_START:
    {
        SampleDataStart start;
        MessageProcess::Unpack(package, start);
        Pipeline2DforNewScan(start);
        Pipeline1DforNewScan(start);

    }
        break;
    case SAMPLE_DATA_DATA:
    {
        SampleDataData data;
        MessageProcess::Unpack(package, data);
        InputToPipeline2D(data);
        InputToPipeline1D(data);

    }
        break;
    case SAMPLE_DATA_END:
    {
        SampleDataEnd end;
        MessageProcess::Unpack(package, end);
        End(end);

    }
        break;
    default:
        break;

    }

    return true;
}

void DataManager::setPipelinPath(const QString &pipeline_path)
{
    _pipeline_path=pipeline_path;
}

bool DataManager::Pipeline1DforNewScan(SampleDataStart &start)
{
    _sample_start = start;

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

bool DataManager::Pipeline2DforNewScan(SampleDataStart &start)
{
    _sample_start = start;
    _rt_pipeline = this->CreatePipeline(QString("config//pipelines//realtime_recon.pipeline"));

    if(_rt_pipeline)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool DataManager::InputToPipeline1D(SampleDataData &data)
{

    auto output_data = CreateIData1D(data);
    if(_rt_pipeline1D)
        _rt_pipeline1D->Input(L"Input", output_data.get());
    return true;

}


bool DataManager::InputToPipeline2D(SampleDataData &data)
{
    //Put the recieved data into the pipeline.
    auto output_data = CreateIData1D(data);
    if(_rt_pipeline)
        _rt_pipeline->Input(L"Input", output_data.get());
    return true;

}


bool DataManager::Load(const QString& file_path)
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
        float x = 3.14;
        // Load fid data and show recon iamges.
        if (!LoadFidRecon(file_path))//xhb->

            return false;
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

bool DataManager::LoadPipeline()
{

    QString pipeline_file = QString("config//pipelines//test0_qt.pipeline");
    Yap::PipelineCompiler compiler;
    auto pipeline = compiler.CompileFile(pipeline_file.toStdWString().c_str());
    QFileInfo info(pipeline_file);
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

    return pipeline->Input(L"Input", nullptr);

}

bool DataManager::LoadFidRecon(const QString& file_path)
{

    QFileInfo file_info(file_path);
    //auto file_name = file_info.baseName();
    auto path = file_info.path();

    /*  if (!ProcessFidfile(file_path, QString("config//pipelines//realtime_recon.pipeline")))
        return false;
    */
    //if (!ProcessFidfile(file_path, QString("D:\\Projects\\Yap2\\API\\ReconDemo\\config\\pipelines\\niumag_recon.pipeline")))
    double x = 3.141;

    if (!ProcessFidfile(file_path, QString("config//pipelines//niumag_recon.pipeline")))
         return false;
    return true;
}


bool DataManager::ProcessFidfile(const QString &file_path, const QString &pipelineFile)
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


Yap::SmartPtr<IProcessor> DataManager::CreatePipeline(const QString& pipelineFile)
{


    Yap::PipelineCompiler compiler;
    auto pipeline = compiler.CompileFile(pipelineFile.toStdWString().c_str());

    QFileInfo info(pipelineFile);
    if (!pipeline)
    {
        QMessageBox::critical(nullptr, QString("Error"), QString("Error compliling the pipeline: ") + info.baseName());
        return Yap::SmartPtr<IProcessor>();
    }

     return pipeline;
}
bool DataManager::Demo2D()
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

            auto output_data = CreateDemoIData2D();
            pipeline->Input(L"Input", output_data.get());
        }
    }
    catch (ConstructError& e)
    {
        //wcout << e.GetErrorMessage() << std::endl;
    }

    return true;
}

Yap::SmartPtr<Yap::IData> DataManager::CreateDemoIData2D()
{
    Yap::Dimensions dimensions;
    unsigned int width = 512;
    unsigned int height = 512;
    unsigned int total_slice_count = 1;

    auto complex_slices = std::shared_ptr<std::complex<float>>(new std::complex<float>[width * height * total_slice_count]);

    auto ushort_slices = new unsigned short[width * height * total_slice_count];

    for(unsigned i = 0; i < total_slice_count; i ++)
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
        (Yap::DimensionSlice, 0U, total_slice_count);



    auto output_data1 = Yap::YapShared(
                new Yap::DataObject<unsigned short>(nullptr, ushort_slices, dimensions, nullptr, nullptr));

    //auto output_data2 = new Yap::DataObject<unsigned short>(nullptr, ushort_slices.get(), dimensions, nullptr, true, nullptr);

    return output_data1;


}
bool DataManager::Demo1D()
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

            auto output_data = CreateDemoIData1D();
            pipeline->Input(L"Input", output_data.get());
        }
    }
    catch (ConstructError& e)
    {
        //wcout << e.GetErrorMessage() << std::endl;
    }

    return true;
}


Yap::SmartPtr<Yap::IData> DataManager::CreateDemoIData1D()
{


    const double PI = 3.1415926;
    int data_vector2_size = 314;
    std::complex<float> * data_vector2 = new std::complex<float>[data_vector2_size];


    double dw = 4*PI / data_vector2_size;
    for(int i = 0; i < data_vector2_size; i ++)
    {

      double temp = data_vector2_size + 10 - i;
      data_vector2[i].real( temp* 10* sin(i * dw ) );
      data_vector2[i].imag( temp* 10* cos(i * dw ) );
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

Yap::SmartPtr<Yap::IData> DataManager::CreateIData1D(SampleDataData &data)
{
    //
    int channel_switch = _sample_start.channel_switch;

    int freq_count = _sample_start.dim1_size;
    int phase_count = _sample_start.dim2_size;
    int slice_count = _sample_start.dim3_size;

    int channel_index = data.rec;

    int phase_index, slice_index;
    calculate_dimindex(_sample_start, data.dim23456_index, phase_index, slice_index);

    //
    int data_size = data.data_value.size();
    assert(phase_count == data_size);
    std::complex<float> * data_vector2 = new std::complex<float>[data_size];

    for(int i = 0; i < data_size; i ++)
    {
      data_vector2[i] = data.data_value[i];
    }

    Yap::Dimensions dimensions;
    dimensions(Yap::DimensionReadout, 0U, freq_count)
        (Yap::DimensionPhaseEncoding, 0U, 1)//
        (Yap::DimensionSlice, 0U, 1);

    //dimensions(Yap::DimensionReadout, 0U, freq_count)
    //    (Yap::DimensionPhaseEncoding, 0U, 1)//
    //    (Yap::DimensionSlice, 0U, 1);

    auto output_data = Yap::YapShared(
                new Yap::DataObject<std::complex<float>>(nullptr, data_vector2, dimensions, nullptr, nullptr));



    if (output_data->GetVariables() == nullptr)
    {
        Yap::VariableSpace variables;
        output_data->SetVariables(variables.Variables());


        variables.AddVariable(L"bool", L"Finished", L"The end of sample data.");
        variables.AddVariable(L"int",  L"channel_switch", L"channel switch.");
        variables.AddVariable(L"int",  L"channel_index", L"channel index.");
        variables.AddVariable(L"int",  L"slice_count", L"slice count.");
        variables.AddVariable(L"int",  L"slice_index", L"slice index.");
        variables.AddVariable(L"int",  L"phase_count", L"phase count.");
        variables.AddVariable(L"int",  L"phase_index", L"phase index.");
        variables.AddVariable(L"int",  L"freq_count",  L"frequency count.");


        variables.Set(L"Finished", false);
        variables.Set(L"channel_switch", channel_switch);
        variables.Set(L"channel_index", channel_index);
        variables.Set(L"slice_count", slice_count);
        variables.Set(L"slice_index", slice_index);
        variables.Set(L"phase_count", phase_count);
        variables.Set(L"phase_index", phase_index);
        variables.Set(L"freq_count", freq_count);

    }

    //test get method.
    VariableSpace variables1(output_data->GetVariables());
    int temp1=variables1.Get<int>(L"channel_index");
    int temp2 = variables1.Get<int>(L"slice_index");
    int temp3=variables1.Get<int>(L"phase_index");
    //
    return output_data;

}

void DataManager::calculate_dimindex(SampleDataStart &start, int dim23456_index, int &phase_index, int &slice_index)
{

    //sending end: dim2 = phase_count, dim3 = slice_count;
    //dim4 maybe echo images or other else.
    int phase_count = start.dim2_size;
    int slice_count = start.dim3_size;
    int dim4_size = start.dim4_size;
    int dim5_size = start.dim5_size;
    int dim6_size = start.dim6_size;
    int dim23456_size = start.dim23456_size;


    assert(dim4_size == 1);
    assert(dim5_size == 1);
    assert(dim6_size == 1);
    assert(phase_count * slice_count == dim23456_size);

    //Decode the order of data received, according to the encoded order in the sender.

    CommonMethod::split_index(dim23456_index, phase_count, slice_count, phase_index, slice_index);




}





bool DataManager::End(SampleDataEnd &end)
{
    Yap::VariableSpace variables;
    variables.AddVariable(L"bool", L"Finished", L"Iteration finished.");
    variables.Set(L"Finished", true);

    auto output = DataObject<int>::CreateVariableObject(variables.Variables(), nullptr);

    if(_rt_pipeline)
    {
        //Send a "Finished" message with no data array to the pipeline.
        _rt_pipeline->Input(L"Input", output.get());
    }
    return true;
}


