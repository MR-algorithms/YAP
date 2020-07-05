#include "Rawdata.h"
#include <assert.h>
#include "Utilities\commonmethod.h"
using namespace Databin;

RawData RawData::s_instance;
///
/// \brief RawData::Prepare
/// \param freqcount
/// \param phasecount
/// \param slicecount
/// \param channel_switch
///
void RawData::Prepare(int scan_id, int freqcount, int phasecount, int slicecount, int channel_switch)
{

    _channel_switch = _channel_switch;
    InitChannels(scan_id, freqcount, phasecount, slicecount, channel_switch);
    _ready = true;

}
///
/// \brief RawData::InitChannels
/// For each channels, Setup the parameters and allocate the memory space.
/// \param freqcount
/// \param phasecount
/// \param slicecount
/// \param channel_switch
/// \return
///
bool RawData::InitChannels(int scan_id, int freqcount, int phasecount, int slicecount, int channel_switch)
{

    int max_channelcount = sizeof(channel_switch) * 8;

    int channel_used = 0;
    for(int index = 0; index < max_channelcount; index ++)
    {
        if(CommonMethod::IsChannelOn(channel_switch, index))
        {
            channel_used ++;
            ChannelData channel_data;
            channel_data.scan_id        = scan_id;
            channel_data.channel_index  = index;
            channel_data.updated        = false;
            channel_data.freq_count     = freqcount;
            channel_data.phase_count    = phasecount;
            channel_data.slice_count    = slicecount;
            channel_data.dim4           = 1;

            channel_data.data.resize(freqcount * phasecount * slicecount);

            memset(channel_data.data.data(), 0, channel_data.data.size() * sizeof(std::complex<float>));

            _channels_data.push_back(channel_data);

        }

    }
    assert(channel_used == CommonMethod::GetChannelCountUsed(channel_switch));

    return true;
}




void RawData::InsertPhasedata(std::complex<float>* source, int length, int channel_index, int slice_index, int phase_index)
{
    assert(_ready);

    int count = 0;
    for(auto it = _channels_data.begin(); it != _channels_data.end(); it++)
    {
        if((*it).channel_index == channel_index)
        {
            count ++;
            assert(count == 1);//Ensure only enter once.
            (*it).updated = true;
            int freq_count = (*it).freq_count;
            int phase_count = (*it).phase_count;
            int slice_size = freq_count * phase_count;

            auto dest_pointer = (*it).data.data()
                    + slice_size * slice_index
                    + freq_count * phase_index;
            memcpy(dest_pointer, source, length * sizeof(std::complex<float>) );
            assert(!CommonMethod::IsComplexelementZero<std::complex<float>>(source, length));


        }
    }


}
/**
  @remark Allocate a storage with specified size and copy the data in databin to the storage.
*/
std::complex<float>* RawData::NewChanneldata(const int channel_index,
                   int& freq_count, int& phase_count, int& slice_count)
{
    assert(channel_index <= MaxChannelIndex());

    std::complex<float>* channel_data;
    int count = 0;
    for(auto item: _channels_data)
    {
        if(item.channel_index == channel_index)
        {
            count ++;
            freq_count = item.freq_count;
            phase_count = item.phase_count;
            slice_count = item.slice_count;

            int length = freq_count * phase_count * slice_count;
            channel_data = new std::complex<float>[length];
            memcpy( channel_data, item.data.data(), length * sizeof(std::complex<float>));

        }
    }
    //Enusre there is one and only one channel with specified index.
    assert(count == 1);
    return channel_data;
}


///
/// \brief RawData::MaxChanneIndex
/// \return
///
int RawData::MaxChannelIndex()
{
    int max_index = -1;
    for(auto channel: _channels_data)
    {
        max_index = (max_index >= channel.channel_index) ? max_index : channel.channel_index;
    }
    return max_index;

}
int RawData::PhaseCount()
{
    assert(_channels_data.size()>0);
    int phase_count = _channels_data[0].phase_count;
    for (auto iter = _channels_data.cbegin(); iter != _channels_data.cend(); iter++)
    {
        assert(phase_count == (*iter).phase_count);
    }
    return phase_count;

}
bool RawData::NeedProcess(const int channel_index)
{
    assert(channel_index >= 0);

    for(auto channel: _channels_data)
    {
        if(channel_index == channel.channel_index)
        {
            return channel.updated;
        }
    }

    return false;
}
