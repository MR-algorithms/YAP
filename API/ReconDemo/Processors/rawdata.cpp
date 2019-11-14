#include "Rawdata.h"
#include <assert.h>
#include "Utilities\commonmethod.h"
using namespace Databin;


///
/// \brief RawData::Prepare
/// \param freqcount
/// \param phasecount
/// \param slicecount
/// \param channel_switch
///
void RawData::Prepare(int freqcount, int phasecount, int slicecount, int channel_switch)
{

    InitChannels(freqcount, phasecount, slicecount, channel_switch);
    _ready = true;

}
///
/// \brief RawData::InitChannels
/// \param freqcount
/// \param phasecount
/// \param slicecount
/// \param channel_switch
/// \return
///
bool RawData::InitChannels(int freqcount, int phasecount, int slicecount, int channel_switch)
{

    int max_channelcount = sizeof(channel_switch) * 8;

    int channel_used = 0;
    for(int index = 0; index < max_channelcount; index ++)
    {
        if(CommonMethod::IsChannelOn(channel_switch, index))
        {
            channel_used ++;
            ChannelData channel_data;
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
  @remark Copy all the channel infomations of a specified channel index to the input variable
  of ChannelData structure.
*/
std::complex<float>* RawData::NewChanneldata(const int channel_index,
                   int& freq_count, int& phase_count, int& slice_count)
{
    assert(channel_index < MaxChannelCount());

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
/// \brief RawData::MaxChannelCount
/// \return
///
int RawData::MaxChannelCount()
{
    int max_index = -1;
    for(auto channel: _channels_data)
    {
        max_index = (max_index >= channel.channel_index) ? max_index : channel.channel_index;
    }
    return (max_index + 1);

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
