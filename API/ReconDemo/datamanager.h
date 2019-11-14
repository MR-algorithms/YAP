#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <vector>
#include <complex>
#include "Interface/Interfaces.h"
#include "MessagePack/datapackage.h"
#include "Interface/IProcessor.h"


namespace Yap{struct IData; }



class DataManager
{
public:
    static DataManager& GetHandle();
    ~DataManager();
    bool Load(const QString& image_path);
    bool LoadPipeline();
    bool Demo1D();
    bool Demo2D();
    bool ReceiveData(DataPackage &package, int cmd_id);
    void setPipelinPath(const QString& pipeline_path);

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
    void calculate_dimindex(SampleDataStart &start, int dim23456, int &phase_index, int &slice_index);

    bool Pipeline2DforNewScan(SampleDataStart &start);

    bool Pipeline1DforNewScan(SampleDataStart &start);
 //

    bool InputToPipeline1D(SampleDataData &data);
    bool InputToPipeline2D(SampleDataData &data);


    bool End(SampleDataEnd &end);

    QString _pipeline_path;


};

#endif // DATAMANAGER_H
