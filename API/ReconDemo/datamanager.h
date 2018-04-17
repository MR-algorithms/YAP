#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <vector>
#include <complex>
#include "Interface/Interfaces.h"
#include "DataSample/datapackage.h"
#include "Interface/IProcessor.h"

namespace Yap{struct IData; }


class DataManager
{
public:
    static DataManager& GetHandle();
    bool Load(const QString& image_path);
    bool Demo1D();
    bool Demo2D();
    bool ReceiveData(DataPackage &package, int cmd_id);

private:
    DataManager();
    static DataManager s_instance;
    SampleDataStart _sample_start;

    Yap::SmartPtr<Yap::IProcessor> _rt_pipeline;
    Yap::SmartPtr<Yap::IProcessor> _rt_pipeline1D;

    Yap::SmartPtr<Yap::IProcessor> CreatePipeline(const QString& pipelineFile);

    bool LoadFidRecon(const QString& file_path);

    bool ProcessFidfile(const QString& file_path, const QString& pipelineFile);

    Yap::SmartPtr<Yap::IData> CreateDemoIData2D();
    Yap::SmartPtr<Yap::IData> CreateDemoIData1D();
    Yap::SmartPtr<Yap::IData> CreateIData1D(SampleDataData &data);
    void calculate_dimindex(SampleDataStart &start, int dim23456, int &dim2_index, int &dim3_index);

    bool Pipeline2DforNewScan(SampleDataStart &start);
    bool Pipeline1DforNewScan(SampleDataStart &start);

    bool InputToPipeline2D(SampleDataData &data);
    bool InputToPipeline1D(SampleDataData &data);
    bool End(SampleDataEnd &end);
    ////
        std::vector<std::complex<float>> receive_phases;
        //SampleDataStart _start_phases;
        int last_phase_index=0;

    ////
    /*
    bool LoadImage(const std::wstring& file_path);
    bool LoadNiuImage(const std::wstring& file_path);
//    bool LoadFid(const std::wstring& file_path);



    void ToModule(unsigned short * dest, std::complex<float> * data, size_t size);

    bool CheckFiles();
    bool CopyFile(const QString& in_file_path, const QString& out_file_path);


    std::vector<std::pair<Yap::SmartPtr<Yap::IData>, Yap::SmartPtr<Yap::IVariableContainer>>> _images;
    Yap::SmartPtr<Yap::IVariableContainer> _properties;
    */
};

#endif // DATAMANAGER_H
