#include "datamanager.h"
#include <cassert>
#include "StringHelper.h"
#include "Implement/DataObject.h"
#include "API/Yap/PipelineCompiler.h"
#include "API/Yap/ProcessorAgent.h"
#include "Implement/CompositeProcessor.h"

#include <algorithm>
#include <qmessagebox>
#include <io.h>
#include <direct.h>
#include <fstream>
#include <QDateTime>

#include <QFileInfo>
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



bool DataManager::LoadFidRecon(const QString& file_path)
{
    QFileInfo file_info(file_path);
    auto file_name = file_info.baseName();
    auto path = file_info.path();

    if (!DoPipeline(file_path, QString("config//pipelines//niumag_recon.pipeline")))
        return false;

    return true;
}


bool DataManager::DoPipeline(const QString &file_path, const QString &pipe)
{

   Yap::PipelineCompiler compiler;
   auto pipeline = compiler.CompileFile(pipe.toStdWString().c_str());

    QFileInfo info(pipe);
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


bool DataManager::Demo()
{
/*
    NiumagFidReader reader;
    SliceIterator slice_iterator;
    DcRemover dc_remover;
    ZeroFilling zero_filling(DestWidth = 512, DestHeight = 512);
    Fft2D fft;
    ModulePhase module_phase;
    DataTypeConvertor convertor;
    NiuMriDisplay2D display2d;

    reader->slice_iterator;
    slice_iterator->dc_remover;
    dc_remover->zero_filling;
    zero_filling->fft;
    fft->module_phase;
    module_phase.Module->convertor;
    convertor.UnsignedShort->display2d;

    self.Input->reader.Input;
*/
/*
    try
    {
        PipelineConstructor constructor;
        constructor.Reset(true);
        constructor.LoadModule(L"BasicRecon.dll");

        constructor.CreateProcessor(L"RecieveData", L"revieve_AComplexImage");
        constructor.CreateProcessor(L"ModulePhase", L"module_phase");

        constructor.CreateProcessor(L"DataTypeConvertor",L"datatype_convertor");

        constructor.CreateProcessor(L"JpegExporter", L"jpeg_exporter");
        constructor.SetProperty(L"jpeg_exporter", L"ExportFolder", L"d:\\output");

        constructor.Link(L"revieve_AComplexImage", L"module_phase");

        //constructor.Link(L"module_phase", L"Module", L"jpeg_exporter", L"Input");

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
*/
    return true;
}

Yap::SmartPtr<Yap::IData> DataManager::CreateDemoIData()
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

/*
bool DataManager::ViewPrescan(const std::wstring& pipe)
{
    PipelineCompiler compiler;
    auto pipeline = compiler.CompileFile(pipe.c_str());

    if (!pipeline)
    {
        QMessageBox::critical(nullptr, QString("Error"), QString("Error compliling the pipeline."));
        return false;
    }

    return pipeline->Input(L"Input", nullptr);
}

bool DataManager::Save(const QString &raw_data_path, bool save_param)
{
    int is_existence = access(raw_data_path.toStdString().c_str(), 0);
    if (is_existence != 0)
    {
        if (mkdir(raw_data_path.toStdString().c_str()) == -1)
            return false;
    }

    if (!CheckFiles())
        return false;

    QFileInfo file_info(raw_data_path);
    auto file_name = file_info.baseName();

    QString o1_file_path = raw_data_path + "/" + file_name + ".sf";
    if (!CopyFile(QString("config//TempResults//temp_o1.sf"), o1_file_path))
        return false;

    QString shimming_file_path = raw_data_path + "/" + file_name + ".shm";
    if (!CopyFile(QString("config//TempResults//temp_shimming.shm"), shimming_file_path))
        return false;

    QString rfa_file_path = raw_data_path + "/" + file_name + ".rfa";
    if (!CopyFile(QString("config//TempResults//temp_rfamp.rfa"), rfa_file_path))
        return false;

    QString scout_file_path = raw_data_path + "/" + file_name + ".scout";
    if (!CopyFile(QString("config//TempResults//temp_scout.scout"), scout_file_path))
        return false;

    QString fid_file_path = raw_data_path + "/" + file_name + ".fid";
    if (!CopyFile(QString("config//TempResults//temp.fid"), fid_file_path))
        return false;

    if (save_param)
    {
        QString params_file_path = raw_data_path + "/" + file_name + ".scan";
        if (!CopyFile(QString("config//TempResults//params.scan"), params_file_path))
            return false;
    }

    return true;
}

bool DataManager::ExportJpeg(Yap::SmartPtr<IData> data, const QString &jpeg_path)
{
    PipelineCompiler compiler;
    auto pipeline = compiler.CompileFile(L"config\\pipelines\\ExportJpeg.pipeline");

    if (!pipeline)
    {
        QMessageBox::critical(nullptr, QString("Error"), QString("Error compliling the ExportJpeg.pipeline."));
        return false;
    }

    auto exporter = pipeline->Find(L"jpeg_exporter");
    if (exporter == nullptr)
        return false;

    auto reader_agent = ProcessorAgent(exporter);
    reader_agent.SetString(L"ExportFolder", jpeg_path.toStdWString().c_str());

    return pipeline->Input(L"Input", data.get());
}

bool DataManager::SetImage(IData* data)
{
    if (!data)
        return false;

    _images.push_back(make_pair(Yap::YapShared(data), YapShared<IVariableContainer>(nullptr)));
    return true;
}

bool DataManager::LoadImage(const std::wstring& file_path)
{
    PipelineCompiler compiler;
    auto pipeline = compiler.CompileFile(L"config\\pipelines\\LoadImage.pipeline");

    if (!pipeline)
    {
        QMessageBox::critical(nullptr, QString("Error"), QString("Error compliling the LoadImage.pipeline."));
        return false;
    }

    auto reader = pipeline->Find(L"reader");
    if (reader == nullptr)
        return false;

    auto reader_agent = ProcessorAgent(reader);
    reader_agent.SetString(L"DataPath", file_path.c_str());

    return pipeline->Input(L"Input", nullptr);
}

bool DataManager::LoadNiuImage(const wstring &file_path)
{
    PipelineCompiler compiler;
    auto pipeline = compiler.CompileFile(L"config\\pipelines\\LoadNiuMriImage.pipeline");

    if (!pipeline)
    {
        QMessageBox::critical(nullptr, QString("Error"), QString("Error compliling the LoadNiuMriImage.pipeline."));
        return false;
    }

    auto reader = pipeline->Find(L"reader");
    if (reader == nullptr)
        return false;

    auto reader_agent = ProcessorAgent(reader);
    reader_agent.SetString(L"DataPath", file_path.c_str());

    return pipeline->Input(L"Input", nullptr);
}




void DataManager::ToModule(unsigned short * dest,
                           std::complex<float> *data,
                           size_t size)
{
    vector<float> module(size);
    for (size_t i = 0; i < size; ++i)
    {
        module[i] = abs(data[i]); //sqrt(data[i].real() * data[i].real() + data[i].imag() * data[i].imag());
    }
    auto max_iter = max_element(module.begin(), module.end());
    for (size_t i = 0; i < size; ++i)
    {
        dest[i] = (unsigned short)(module[i] / *max_iter * 65535);
    }
}

bool DataManager::CheckFiles()
{
    QFileInfo o1_info("config//TempResults//temp_o1.sf");
    QFileInfo shimming_info("config//TempResults//temp_shimming.shm");
    QFileInfo rfa_info("config//TempResults//temp_rfamp.rfa");
    QFileInfo scout_info("config//TempResults//temp_scout.scout");
    QFileInfo fid_info("config//TempResults//temp.fid");

    if (fid_info.lastModified() < o1_info.lastModified() ||
            fid_info.lastModified() < shimming_info.lastModified() ||
            fid_info.lastModified() < rfa_info.lastModified() ||
            fid_info.lastModified() < scout_info.lastModified())
    {
        QMessageBox::warning(nullptr, QString("Warning"), QString("Please do prescan and scout before scan!"),
                             QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }

    return true;
}

bool DataManager::CopyFile(const QString& in_file_path, const QString& out_file_path)
{
    QFileInfo in_info(in_file_path);
    if (!in_info.isFile())
    {
        QMessageBox::warning(nullptr, QString("Warning"), QString("No file: ") + in_file_path,
                             QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }

    std::ifstream in(in_file_path.toStdWString(), ios::binary);
    std::ofstream out(out_file_path.toStdWString(), ios::binary);
    out << in.rdbuf();

    return true;
}

*/
