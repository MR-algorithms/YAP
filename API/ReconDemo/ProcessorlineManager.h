#ifndef PROCESSLINEMANAGER_H
#define PROCESSLINEMANAGER_H

#include <QString>
#include <vector>
#include <complex>
#include "Interface/Interfaces.h"
#include "MessagePack/datapackage.h"
#include "Interface/IProcessor.h"
#include "QDataEvent.h"


namespace Yap{struct IData; }
using namespace ScanInfo;

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
    bool OnScanStart(const ScanDimension& scan_dim);
    bool OnChannelPhasestep(const ScanDimension& scan_dim, shared_ptr<complex<float>> buffer_pointer );
    bool OnScanFinished(const ScanDimension& scan_dim);

private:
    ProcessorlineManager();
    static ProcessorlineManager s_instance;

    Yap::SmartPtr<Yap::IProcessor> CreatePipeline(const QString& pipelineFile);

    bool Pipeline1DforNewScan();
    bool Pipeline2DforNewScan();
    bool InputPipeline1D(const ScanDimension& scan_dim, shared_ptr<complex<float>> buffer_pointer);
    bool InputPipeline2D(const ScanDimension& scan_dim, shared_ptr<complex<float>> buffer_pointer);

    bool ProcessFidfile(const QString& file_path, const QString& pipelineFile);
    Yap::SmartPtr<Yap::IData> CreateDemoData1D(int phase_index = 100, double phi0=0);
    Yap::SmartPtr<Yap::IData> CreateDemoData2D(int &width, int &height, int &slice_count);
    Yap::SmartPtr<Yap::IData> CreateData1D(const ScanDimension& scan_dim, shared_ptr<complex<float>> buffer_pointer);

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
