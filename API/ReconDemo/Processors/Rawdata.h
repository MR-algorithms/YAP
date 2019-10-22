#ifndef RAWDATA_H
#define RAWDATA_H
#include <map>
#include <complex>
#include <vector>
///
///
///
namespace Databin{
///
/// \brief The ChannelData struct
///
struct ChannelData
{
    int channel_index;
    //To be revised:
    //A: Update state: 2 possible schemes:
    //1, store every slice state which denotes if it is updated.
    //2, only one state vairabe, but the state should be set false between
    //beginning and ending of the writing.
    //B: Sample state: It should be recorded that If the raw data is really existance in every phase step.
    bool updated;
    int freq_count;
    int phase_count;
    int slice_count;
    int dim4;
    std::vector<std::complex<float>> data;
    ChannelData():
        channel_index(0),
        updated(false),
        freq_count(0),
        phase_count(0),
        slice_count(0),
        dim4(0)
    {
        data.resize(0);
    }
    ChannelData(const ChannelData &rhs)
    {
        channel_index = rhs.channel_index;
        updated = rhs.updated;
        freq_count = rhs.freq_count;
        phase_count = rhs.phase_count;
        slice_count = rhs.slice_count;
        dim4 = rhs.dim4;
        data.assign(rhs.data.begin(),rhs.data.end());
    }

    ChannelData& operator = (const ChannelData& rhs){
        channel_index = rhs.channel_index;
        updated = rhs.updated;
        freq_count = rhs.freq_count;
        phase_count = rhs.phase_count;
        slice_count = rhs.slice_count;
        dim4 = rhs.dim4;
        data.assign(rhs.data.begin(),rhs.data.end());
        return *this;
    }



};

class RawData
{

public:

    RawData(): _ready(false) {}
    void Prepare(int freqcount, int phasecount, int slicecount, int channel_switch);
    bool Ready(){return _ready;}
    void InsertPhasedata(std::complex<float>* source, int length, int channel_index, int slice_index, int phase_index);
    int MaxChannelCount();
    bool NeedProcess(const int channel_index);
    std::complex<float>* NewChanneldata(const int channel_index, int& freq_count, int& phase_count, int& slice_count);

private:
    bool InitChannels(int freqcount, int phasecount, int slicecount, int channel_switch);
    bool IsChannelOn(int channel_switch, int channel_index);

    //An element of vector<ChannelData> stores all infomations and
    //data of a channel.
    //Note that the index of the vector element doesnot equal the channel index.
    std::vector<ChannelData> _channels_data;
    bool _ready;
};

}
#endif // RAWDATA_H
