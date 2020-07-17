#ifndef RECIEVEDATA_H
#define RECIEVEDATA_H

#include "Implement/ProcessorImpl.h"
#include<thread>

class ReceiveData : public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(ReceiveData)
public:
    explicit ReceiveData();
    ReceiveData(const ReceiveData &rhs);

private:
    virtual ~ReceiveData();

    virtual bool Input(const wchar_t *name, Yap::IData *data) override;

    void Threadfunc_running();

    bool IsFinished(Yap::IData *data) const;

    Yap::SmartPtr<Yap::IData> CreateRefIData();
    bool CreateOutData(std::complex<float>* channel_data, int channel_index,
                       int freq_count, int phase_count, int slice_count);

    std::shared_ptr<std::thread> _thread_runing;


};

#endif // RECIEVEDATA_H
