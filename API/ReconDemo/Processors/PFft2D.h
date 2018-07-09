#ifndef PFFT2D_H
#define PFFT2D_H

#include "Implement/ProcessorImpl.h"
#include "Client/DataHelper.h"
//#include "D:/ThirdParty/FFTW/include/fftw3.h"
#include "fftw3.h"
#include <complex>
#include <future>
#include<thread>
#include<QDebug>
#include<utility>
#include<functional>
#include<chrono>
#include<mutex>
#include"globalvariable.h"

using namespace std;
using namespace Yap;

class PFft2D :
    public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(PFft2D)
public:
    explicit PFft2D();
    PFft2D(const PFft2D& rhs);
    static void fft2d_thread2(PFft2D* pFft2D,Yap::IData* myData);
    static void fft2d_thread(PFft2D* pFft2D);//,std::promise<bool> &promiseObj);//线程函数
    static void fft2d_thread0();
    void ReceiveDataFromOtherThread(Yap::IData *datain,PFft2D* p);//父线程向子线程传入数据并更新

    void JoinMyThread(){_mythread.join();}
    bool ToFft2D(Yap::IData * data);
    std::vector<std::complex<float> > LookintoPtr(std::complex<float> *data, int size, int start, int end);
protected:
    ~PFft2D();

    virtual bool Input(const wchar_t * port,Yap::IData * data) override;
    bool CheckIData(Yap::IData * data);
    //bool ToFft2D(Yap::IData * data);
    void FftShift(std::complex<float>* data, size_t width, size_t height);
    void SwapBlock(std::complex<float> * block1, std::complex<float> * block2, size_t width, size_t height, size_t line_stride);

    unsigned int _plan_data_width;
    unsigned int _plan_data_height;
    bool _plan_inverse;
    bool _plan_in_place;
    fftwf_plan _fft_plan;

    //
    int _count=0;//用于判断是否第一次进入

    std::thread _mythread;//用于创建线程

    Yap::IData* newIData;//最新传入的数据

    std::vector<Yap::IData*> _allData;//用于存放所有通道所有层的数据

    std::mutex dataCacheMutex;
    //

    bool Fft(std::complex<float> * data, std::complex<float> * result, size_t width, size_t height, bool inverse = false);
    void Plan(size_t width, size_t height, bool inverse, bool in_place);
};

class Scoped_thread
{
    std::thread t;
public:
    explicit Scoped_thread(std::thread t_):t(move(t_))
    {
       if(!t.joinable())
           throw std::logic_error("No thread");
    }
    ~Scoped_thread()
    {
       t.join();
    }
   Scoped_thread(Scoped_thread const&)=delete;
   Scoped_thread& operator=(Scoped_thread const&)=delete;

};

#endif // PFFT2D_H
