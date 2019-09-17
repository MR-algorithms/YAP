#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <vector>
#include <complex>
#include "Interface/Interfaces.h"
#include "DataSample/datapackage.h"
#include "Interface/IProcessor.h"

#include <future>
#include<thread>
#include<QDebug>
#include<utility>
#include<functional>
#include<chrono>

namespace Yap{struct IData; }


enum EThreadState{ idle, scanning, paused, finished};
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
    bool Pipeline2DforNewScan();
//
    int GetChannelCountInMask(unsigned int channelMask);
    bool InSertPhasedata(Yap::IData *data);
    int GetChannelIndexInMask(unsigned int channelMask, int channelIndex);
    std::vector<std::complex<float>> LookintoPtr(std::complex<float> *data, int size, int start, int end);
    bool IsFinished(Yap::IData *data);
    static void reconstruction_thread(DataManager* datamanager,std::promise<bool> &promiseObj);
    static void reconstruction_thread2();
    void SetPwnd( QWidget* pwnd);
    QWidget* GetPwnd(){return _pwnd;}
    void JoinThread();
    void SetSampleDataDataCount(const int& sampleDataData_count){_sampleDataData_count=sampleDataData_count;}
//
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

    // bool Pipeline2DforNewScan(SampleDataStart &start);

    bool Pipeline1DforNewScan(SampleDataStart &start);
 //
    bool SetSampleStart(SampleDataStart &start);

    bool InputToPipeline2D(SampleDataData &data);
    bool InputToPipeline1D(SampleDataData &data);
    bool End(SampleDataEnd &end);

    QString _pipeline_path;
    ////
        std::vector<std::complex<float>> receive_phases;
        //SampleDataStart _start_phases;
        int last_phase_index=0;
        std::thread _mythread;

        std::promise<bool> promiseObj;
        std::future<bool> futureObj = promiseObj.get_future();

        Yap::SmartPtr<Yap::IData> output_data;

        int _sampleDataData_count;//表示接受到的SAMPLE_DATA_DATA的次数，一次计数为一通道一层一相位编码的数据

        QWidget* _pwnd;

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
