#ifndef RECIEVEDATA_H
#define RECIEVEDATA_H

#include "Implement/ProcessorImpl.h"



struct RawDataInfoB
{
    unsigned int freq_count;
    unsigned int phase_count;
    unsigned int slice_count;
    unsigned int dim4;
    unsigned int dim5;
    unsigned int dim6;
    unsigned int channel_mask;
    //unsigned int dim23456;

    RawDataInfoB() : freq_count(0), phase_count(0), slice_count(0), dim4(0), dim5(0), dim6(0), channel_mask(0) {}
    bool operator == (const RawDataInfoB& rhs) const{
        return freq_count == rhs.freq_count &&
            phase_count == rhs.phase_count &&
            slice_count == rhs.slice_count &&
            dim4 == rhs.dim4 &&
            dim5 == rhs.dim5 &&
            dim6 == rhs.dim6 &&
            channel_mask == rhs.channel_mask;
    }
};

class RecieveData : public Yap::ProcessorImpl
{
    IMPLEMENT_SHARED(RecieveData)
public:
    explicit RecieveData();
    RecieveData(const RecieveData &rhs);

private:
    virtual ~RecieveData();

    virtual bool Input(const wchar_t *name, Yap::IData *data) override;
    bool IsFinished(Yap::IData *data);
    bool InitScanData(Yap::IData *data);

    bool InsertPhasedata(Yap::IData *data);
    bool InsertPhasedata(std::complex<float> *data, int channel_index, int slice_index, int phase_index);
    bool CreateOutData();
    int GetChannelCountInMask(unsigned int channelMask);
    int GetChannelIndexInMask(unsigned int channelMask, int channelIndex);
    void TestWhatcanbeTested();
    std::shared_ptr<std::complex<float>> _data;
    //std::vector<std::complex<float>> _datav;
    RawDataInfoB _dataInfo;
    //bool _init;

    std::vector<std::complex<float>> LookintoPtr(std::complex<float> *data, int size, int start, int end);
};

#endif // RECIEVEDATA_H
