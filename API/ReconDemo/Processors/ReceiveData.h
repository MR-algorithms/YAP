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
	void Forward(Yap::IData *data);
	//OnTimer();
	void Threadfunc_running();

    bool IsFinished(Yap::IData *data) const;

    Yap::SmartPtr<Yap::IData> CreateOutData(Yap::IData* data, std::complex<float>* channel_raw_data, int channel_index,
                       int freq_count, int phase_count, int slice_count);

    std::shared_ptr<std::thread> _thread_runing;


};

#endif // RECIEVEDATA_H
