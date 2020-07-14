#include "Rawdata.h"
#include <assert.h>
#include "Utilities\commonmethod.h"
//#include <algorithm>
//#include <vector>
//#include <iterator>
using namespace Databin;

RawData RawData::s_instance;

//check the state of phase steps of all slices.
//0: no data;
//1: all slices datas of the current phase step have been inserted.
//2: part of slices datas of the current phase step have been inserted.
int ChannelData::StateOfPhasesteps()
{
    assert(slice_count > 0);
    assert(phase_count > 0);
    assert(phase_index >= 0);
    std::vector<int> slices;
    slices.resize(slice_count);

    for(int i=0; i < slice_count; i++)
    {
        slices[i] = phasemask_slices[i * phase_count + phase_index];
    }
    int state = 0;
    auto result = std::find(slices.begin(), slices.end(), 1);
    if(result == slices.end())//cannot find 1.
    {
        state = 0;
    }
    else
    {
        result = std::find(slices.begin(), slices.end(), 0);
        if(result == slices.end())//cannot find 0.
        {
            state = 1;
        }
        else
            state = 2;
    }

    return state;
}

void ChannelData::SetPhasesteps(int slice_index, int phase_index)
{
    assert(slice_count > 0);
    assert(phase_count > 0);
    assert(phase_index >= 0);

    phasemask_slices[slice_index * phase_count + phase_index] = 1;


}

///
/// \brief RawData::Prepare
/// \param freqcount
/// \param phasecount
/// \param slicecount
/// \param channel_switch
///
/// freqCount: data count in the frequency direction.
void RawData::Prepare(int scan_id, int freqcount, int phasecount, int slicecount, int channel_switch)
{

    InitChannels(scan_id, freqcount, phasecount, slicecount, channel_switch);
    NotifyObserver(SSScanStart, scan_id, 0, 0);
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
            channel_data.channel_switch = channel_switch;
            channel_data.channel_index  = index;
             channel_data.freq_count     = freqcount;
            channel_data.phase_count    = phasecount;
            channel_data.slice_count    = slicecount;
            channel_data.dim4           = 1;

            channel_data.data.resize(freqcount * phasecount * slicecount);
            channel_data.phasemask_slices.resize(phasecount * slicecount);

            memset(channel_data.data.data(), 0, channel_data.data.size() * sizeof(std::complex<float>));
            memset(channel_data.phasemask_slices.data(), 0, channel_data.phasemask_slices.size() * sizeof(int));

            _channels_data.push_back(channel_data);

        }

    }
    assert(channel_used == CommonMethod::GetChannelCountUsed(channel_switch));

    return true;
}

//InsertPhaseData of one slice.
void RawData::InsertPhasedata(std::complex<float>* source, int length, int channel_index, int slice_index, int phase_index)
{
    assert(_ready);

    int count = 0;
    for(auto it = _channels_data.begin(); it != _channels_data.end(); it++)
    {
        //find the right channel to insert phase data.
        if((*it).channel_index == channel_index)
        {
            //check validation.
            count ++;
            assert(count == 1);
            int state =(*it).StateOfPhasesteps();
            if(state==0)//start writing.
            {
                (*it).phase_index = phase_index;
            }
            else if(state==2)//writing
            {
                assert((*it).phase_index==phase_index);

            }
            else
            {
                assert(state==1);

            }
            //end of check.

            int freq_count = (*it).freq_count;
            int phase_count = (*it).phase_count;
            int slice_size = freq_count * phase_count;

            auto dest_pointer = (*it).data.data()
                    + slice_size * slice_index
                    + freq_count * phase_index;
            memcpy(dest_pointer, source, length * sizeof(std::complex<float>) );
            assert(!CommonMethod::IsComplexelementZero<std::complex<float>>(source, length));

            (*it).SetPhasesteps(slice_index, phase_index);
            state = (*it).StateOfPhasesteps();
            if(state==1)
            {
                NotifyObserver(SSChannelPhaseStep, (*it).scan_id, channel_index, phase_index);
            }
            else
            {
                assert(state==2);
                int xx = 3;
            }

        }
    }

}
/**
    @remark Create a complex buffer filled with a channel data from data databin,
    which is to be used in reconstruction pipeline.
*/
std::complex<float>* RawData::NewChanneldata(int channel_index,
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

//Create a new complex buffer with the current phase step data.
std::complex<float>* RawData::NewPhaseStep(int channel_index, int phase_index, int display_count)
{
    assert(channel_index <= MaxChannelIndex());


    std::complex<float>* display_buffer;
    int count = 0;
    for(auto &item: _channels_data)
    {
        if(item.channel_index == channel_index)
        {
            count ++;
            int freq_count = item.freq_count;
            int phase_count = item.phase_count;
            int slice_count = item.slice_count;
            //item.slice_count = 13; //source data can be written, while iterating by reference.

            assert(display_count>0 && display_count<freq_count*slice_count);
            display_buffer = new std::complex<float>[display_count];
            int left = display_count;
            for(int i=0; i<slice_count; i++)//Data count different from phase count is supported.
            {
                int length = (left<freq_count)? left:freq_count;
                assert(length>0);
                int slice_size = freq_count*phase_count;

                memcpy( display_buffer + i*freq_count,
                        item.data.data()+i*slice_size + phase_index*freq_count,
                        length * sizeof(std::complex<float>));
                assert(!CommonMethod::IsComplexelementZero<std::complex<float>>(display_buffer, length));
                left -= length;
                if(left==0)
                    break;
            }
            assert(left==0);
        }
    }
    //Enusre there is one and only one channel with specified index.
    assert(count == 1);
    return display_buffer;

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

const ChannelData& RawData::GetChannelData(int channel_index) const
{
    assert(_channels_data.size()>0);
    const ChannelData& result = _channels_data[channel_index];

    int phase_count = _channels_data[0].phase_count;
    for (auto iter = _channels_data.cbegin(); iter != _channels_data.cend(); iter++)
    {
        assert(phase_count == (*iter).phase_count);
    }
    return result;
}
bool RawData::NeedProcess(const int channel_index)
{
    assert(channel_index >= 0);

    for(auto channel: _channels_data)
    {
        if(channel_index == channel.channel_index)
        {
            int state = channel.StateOfPhasesteps();
            return(state==1);
        }
    }

    return false;
}

void RawData::NotifyObserver(ScanSignal scan_signal, uint32_t scan_id, int channel_index, int phase_index)
{
    _observer->OnData(scan_signal, scan_id, channel_index, phase_index);
    return;
}


void RawData::SetObserver(std::shared_ptr<IDataObserver> observer)
{
    //boost::unique_lock<boost::mutex> lk(_state_mutex);
    _observer = observer;
}
