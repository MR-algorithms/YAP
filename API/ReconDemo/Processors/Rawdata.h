#ifndef RAWDATA_H
#define RAWDATA_H
#include <map>
#include <complex>
#include <vector>
#include <memory>

#include "QDataEvent.h"

/// Receiving data and storing data, while notifying the corresponding user.
///


///
///
///1£¬Reconstruction is a time consuming process, it should be implemented with independent thread;
///2, 1D data display can be implemented with notifying mode.
///3, Databin shall maintain which phase steps of a channel can be used to be feeded into pipeline.
///4, In the first step, we consider 1D data display while storing a phase step.
namespace Databin{
///
/// \brief The ChannelData struct
///
struct ChannelData
{
    int scan_id;
    int channel_switch;
    int channel_index;
    int phase_index;
    //Update state: store every slice state which denotes if it is updated.
    int freq_count;
    int phase_count;
    int slice_count;
    int dim4;
    std::vector<std::complex<float>> data;
    std::vector<int> phasemask_slices; //recording current phase mask of all slices.
    ChannelData():
        scan_id(0),
        channel_switch(0),
        channel_index(0),
        phase_index(0),
        freq_count(0),
        phase_count(0),
        slice_count(0),
        dim4(0)
    {
        data.resize(0);
        phasemask_slices.resize(0);
    }
    ChannelData(const ChannelData &rhs)
    {
        scan_id = rhs.scan_id;
        channel_switch = rhs.channel_switch;
        channel_index = rhs.channel_index;
        phase_index = rhs.phase_index;
        freq_count = rhs.freq_count;
        phase_count = rhs.phase_count;
        slice_count = rhs.slice_count;
        dim4 = rhs.dim4;
        data.assign(rhs.data.begin(),rhs.data.end());
        phasemask_slices.assign(rhs.phasemask_slices.begin(), rhs.phasemask_slices.end());
    }

    ChannelData& operator = (const ChannelData& rhs){
        scan_id = rhs.scan_id;
        phasemask_slices = rhs.phasemask_slices;
        channel_index = rhs.channel_index;
        phase_index = rhs.phase_index;
        freq_count = rhs.freq_count;
        phase_count = rhs.phase_count;
        slice_count = rhs.slice_count;
        dim4 = rhs.dim4;
        data.assign(rhs.data.begin(),rhs.data.end());
        phasemask_slices.assign(rhs.phasemask_slices.begin(), rhs.phasemask_slices.end());
        return *this;
    }
    int StateOfPhasesteps();
    void SetPhasesteps(int slice_index, int phase_index);


};
//RawData should have some properties:
//1, singleton;
//2, different thread can access it safely.
//3, multi protocol.
//4, no thread function.
class RawData
{

public:
    static RawData& GetHandle() {return s_instance;}
    void SetObserver(std::shared_ptr<IDataObserver> observer);
    void Prepare(int scan_id, int freqcount, int phasecount, int slicecount, int channel_switch);
    bool Ready(){return _ready;}
    void InsertPhasedata(std::complex<float>* source, int length, int channel_index, int slice_index, int phase_index);
    int MaxChannelIndex();

    const ChannelData& GetChannelData(int channel_index) const;

    bool NeedProcess(const int channel_index);
    std::complex<float>* NewChanneldata(int channel_index, int& freq_count, int& phase_count, int& slice_count);
    std::complex<float>* NewPhaseStep(int channel_index, int phase_index, int display_count);
private:

    RawData(): _ready(false) {}

    bool InitChannels(int scan_id, int freqcount, int phasecount, int slicecount, int channel_switch);
    void NotifyObserver(ScanSignal scan_signal, uint32_t scan_id, int channel_index, int phase_index);

    static RawData s_instance;

    //An element of vector<ChannelData> stores all infomations and
    //data of a channel.
    //Note that the index of the vector element doesnot equal the channel index.
    std::vector<ChannelData> _channels_data;
    int _current_channel_index;
    int _current_slice_index;

    std::shared_ptr<IDataObserver> _observer;
    bool _ready;
};

}
#endif // RAWDATA_H
