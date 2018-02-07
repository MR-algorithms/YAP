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
    bool RecieveData(DataPackage &package, int cmd_id);

private:
    DataManager();
    static DataManager s_instance;
    SampleDataStart _sample_start;

    Yap::SmartPtr<Yap::IProcessor> _rt_pipeline;

    Yap::SmartPtr<Yap::IProcessor> CreatePipeline(const QString& pipelineFile);

    bool LoadFidRecon(const QString& file_path);

    bool ProcessFidfile(const QString& file_path, const QString& pipelineFile);

    Yap::SmartPtr<Yap::IData> CreateDemoIData2D();
    Yap::SmartPtr<Yap::IData> CreateDemoIData1D();
    Yap::SmartPtr<Yap::IData> CreateIData1D(SampleDataData &data);
    void calculate_dimindex(SampleDataStart &start, int dim23456, int &dim2_index, int &dim3_index);

    bool NewScan(SampleDataStart &start);
    bool InputToPipeline(SampleDataData &data);
    bool DrawData1D(SampleDataData &data);
    bool End(SampleDataEnd &end);

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
