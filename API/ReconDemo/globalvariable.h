#ifndef GLOBALVARIABLE_H
#define GLOBALVARIABLE_H
#include <mutex>
#include <condition_variable>
#include <complex>
#include <vector>
#include "Interface/IProcessor.h"
#include"DataSample/reconserver.h"
#include"DataSample/SampleDataProtocol.h"


typedef struct AllDataRepository
{
    std::shared_ptr<ReconServer> reconServer1;
    std::mutex gv_mtx;//全局互斥锁
    std::condition_variable gv_cv;//全局条件变量
    bool gv_ready;//全局标志位

    bool gv_is_finished;//用于判断是否接收结束以便通知子线程进行最后一次重建图像
    int gv_channel_count;//通道总数
    unsigned int received_phase_count;//已经接收到的行总数，实际总行数=received_phase_count*_gv_sample_start.dim3_size*gv_channel_count
    unsigned int reconstruct_phase_count;
    std::vector<std::complex<float> > _all_data;//存储的所有数据
//    std::complex<float>* _all_data;
    Yap::SmartPtr<Yap::IProcessor> _rt_pipeline;//重建所需的流水线
    SampleDataStart _gv_sample_start;//主要通过一些维度信息

    AllDataRepository():gv_ready(false),gv_is_finished(true){}

}DataRepository;

extern"C" DataRepository gv_data_repopsitory;

//extern"C" std::mutex gv_mtx;//全局互斥锁

//extern"C" std::condition_variable gv_cv;//全局条件变量

//extern"C" bool gv_ready;//全局标志位

#endif // GLOBALVARIABLE_H
