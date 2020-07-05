#ifndef PROCESSLINEMANAGER_H
#define PROCESSLINEMANAGER_H

#include <QString>
#include <vector>
#include <complex>
#include "Interface/Interfaces.h"
#include "MessagePack/datapackage.h"
#include "Interface/IProcessor.h"


namespace Yap{struct IData; }



class ProcessorlineManager
{
public:
    static ProcessorlineManager& GetHandle();
    ~ProcessorlineManager();
    //
    bool Demo1D();
    bool Demo2D();
    void RunPipeline(const QString& pipeline);
    bool LoadData(const QString& image_path);
    //
    bool OnDataBegin(int scan_id);
    bool OnDataData(int scan_id);
    bool OnDataEnd(int scan_id);

private:
    ProcessorlineManager();
    static ProcessorlineManager s_instance;

    Yap::SmartPtr<Yap::IProcessor> CreatePipeline(const QString& pipelineFile);
    bool ProcessFidfile(const QString& file_path, const QString& pipelineFile);
    Yap::SmartPtr<Yap::IData> CreateDemoIData2D(int &width, int &height, int &slice_count);
    Yap::SmartPtr<Yap::IData> CreateDemoIData1D();


    bool Pipeline2DforNewScan(int scan_id);
    bool Pipeline1DforNewScan(int scan_id);

    void AddVariables(Yap::IData *idata,
                      int phase_count,
                      int slice_index,
                      int slice_count,
                      int channel_switch,
                      int channel_index
                      );

    Yap::SmartPtr<Yap::IProcessor> _rt_pipeline1D;
    Yap::SmartPtr<Yap::IProcessor> _rt_pipeline2D;


};

#endif
