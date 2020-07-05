#ifndef RECIEVEDATA_H
#define RECIEVEDATA_H

#include "Implement/ProcessorImpl.h"


class ReceiveData : public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(ReceiveData)
public:
    explicit ReceiveData();
    ReceiveData(const ReceiveData &rhs);

private:
    virtual ~ReceiveData();

    virtual bool Input(const wchar_t *name, Yap::IData *data) override;

    bool IsFinished(Yap::IData *data) const;

    Yap::SmartPtr<Yap::IData> CreateRefIData();
    bool CreateOutData(std::complex<float>* channel_data, int channel_index,
                       int freq_count, int phase_count, int slice_count);


};

#endif // RECIEVEDATA_H
