#ifndef RECIEVEDATA_H
#define RECIEVEDATA_H

#include "Implement/ProcessorImpl.h"
#include "RawData.h"



class ReceiveData : public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(ReceiveData)
public:
    explicit ReceiveData();
    ReceiveData(const ReceiveData &rhs);

private:
    virtual ~ReceiveData();

    virtual bool Input(const wchar_t *name, Yap::IData *data) override;
    bool StoreData(Yap::IData *data);
    bool ProcessData();

    bool IsFinished(Yap::IData *data) const;
    bool InitScanData(Yap::IData *data);

    bool InsertPhasedata(Yap::IData *data);
    bool InsertPhasedata(std::complex<float> *data, int channel_index, int slice_index, int phase_index);
    Yap::SmartPtr<Yap::IData> CreateRefIData();
    bool CreateOutData(std::complex<float>* channel_data, int channel_index,
                       int freq_count, int phase_count, int slice_count);

    Databin::RawData _rawdata;
    Yap::IData * _ref_data;

};

#endif // RECIEVEDATA_H
