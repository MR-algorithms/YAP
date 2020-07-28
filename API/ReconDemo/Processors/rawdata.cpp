#include "Rawdata.h"
#include <assert.h>
#include "Utilities\commonmethod.h"
#include "QtUtilities/FormatString.h"
//#include <algorithm>
//#include <vector>
//#include <iterator>
using namespace Databin;

RawData RawData::s_instance;


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
			channel_data.processed = true;

            assert(channel_data.datax==nullptr);
            int channel_size = freqcount*phasecount * slicecount;
            channel_data.datax = new std::complex<float>[channel_size];
            channel_data.phasemask_slices = new bool[phasecount * slicecount];

            memset(channel_data.datax, 0, channel_size * sizeof(std::complex<float>));
            memset(channel_data.phasemask_slices, 0, phasecount*slicecount * sizeof(bool));

            _channels_data.push_back(channel_data);

        }

    }
    assert(channel_used == CommonMethod::GetChannelCountUsed(channel_switch));

    return true;
}

//InsertPhaseData of one slice.
void RawData::InsertPhasedata(std::complex<float>* source, int length, 
	int channel_index, int slice_index, int phase_index)
{
    assert(_ready);
	

	auto it = std::find_if(_channels_data.begin(), _channels_data.end(), [&](ChannelData &item) {
		return (channel_index == item.channel_index);
	});

	if(it != _channels_data.end())
	{ 
		assert(length == (*it).freq_count);
		UpdateChannel(*it, source, length, channel_index, slice_index, phase_index);
	}
	

}

void RawData::UpdateChannel(ChannelData& channel_data, const complex<float>* data, const int freq_count,
	const int channel_index, const int slice_index, const int phase_index)
{
	int state = StateOfPhasesteps(channel_data, phase_index);
	if (state == 0)//start writing.
	{
		channel_data.phase_index = phase_index;
	}
	else if (state == 2)//writing
	{
		assert(channel_data.phase_index == phase_index);

	}
	else
	{
		assert(0);//

	}
	assert(freq_count == channel_data.freq_count);
	int phase_count = channel_data.phase_count;
		
	auto dest_pointer = channel_data.datax
		+ freq_count*phase_count * slice_index
		+ freq_count * phase_index;
	memcpy(dest_pointer, data, freq_count * sizeof(std::complex<float>));

	SetPhasestep(channel_data, channel_index, slice_index, phase_index);
	
	state = StateOfPhasesteps(channel_data, phase_index);
	if (state == 1)//if There's new ready data.
	{
		channel_data.processed = false;
	}
	else
	{
		assert(state == 2);
	}

}

//check the state of phase steps of all slices.
//0: no data;
//1: all slices datas of the current phase step have been inserted.
//2: part of slices datas of the current phase step have been inserted.
int RawData::StateOfPhasesteps(const ChannelData& channel_data, int phase_index)
{
	int slice_count = channel_data.slice_count;
	int phase_count = channel_data.phase_count;
	assert(slice_count > 0);
	assert(phase_count > 0);
	assert(phase_index >= 0);
	std::vector<int> slices;
	slices.resize(slice_count);

	for (int i = 0; i < slice_count; i++)
	{
		slices[i] = *(channel_data.phasemask_slices + i * phase_count + phase_index);
	}
	int state = 0;
	auto result = std::find(slices.begin(), slices.end(), 1);
	if (result == slices.end())//cannot find 1.
	{
		state = 0;
	}
	else
	{
		result = std::find(slices.begin(), slices.end(), 0);
		if (result == slices.end())//cannot find 0.
		{
			state = 1;
		}
		else
			state = 2;
	}

	return state;
}

void RawData::SetPhasestep(ChannelData& channel_data, int channel_index, int slice_index, int phase_index)
{
	assert(channel_data.channel_index == channel_index);
	int slice_count = channel_data.slice_count;
	int phase_count = channel_data.phase_count;

	assert(slice_count > 0);
	assert(phase_count > 0);
	assert(phase_index >= 0);

	*(channel_data.phasemask_slices + slice_index * phase_count + phase_index) = true;


}

int RawData::GetNumofReadyphasesteps(int channel_index)
{
	auto it = std::find_if(_channels_data.begin(), _channels_data.end(), [&](ChannelData &item) {
		return (channel_index == item.channel_index);
	});
	int num = 0;
	if (it != _channels_data.end())
	{
		vector<bool> ready_phase_steps = GetReadyPhasesteps(*it);

		for (int i = 0; i < ready_phase_steps.size(); i++)
		{
			if (ready_phase_steps[i])
				num += 1;
		}
	}
	return num;
}

vector<bool> RawData::GetReadyPhasesteps(const ChannelData& channel_data)
{
	assert(channel_data.phase_count > 0);
	vector<bool> ready_phase_steps;
	ready_phase_steps.resize(channel_data.phase_count);

	for (int i = 0; i < ready_phase_steps.size(); i++)
	{
		ready_phase_steps[i] = (1==StateOfPhasesteps(channel_data, i));
	}
	return ready_phase_steps;
}
/**
    @remark Create a complex buffer filled with a channel data from data databin,
    which is to be used in reconstruction pipeline.
*/
std::complex<float>* RawData::GetChannelRawdata(int channel_index,
                   int& freq_count, int& phase_count, int& slice_count)
{
    
	auto it = std::find_if(_channels_data.begin(), _channels_data.end(), [&](ChannelData &item) {
		return (channel_index == item.channel_index);
	});

	std::complex<float>* channel_raw_data;
    int count = 0;
    if(it!=_channels_data.end())
    {
     
		if ((*it).processed == true)
			return nullptr;
        
        freq_count = (*it).freq_count;
        phase_count = (*it).phase_count;
        slice_count = (*it).slice_count;

        int length = freq_count * phase_count * slice_count;
        channel_raw_data = new std::complex<float>[length];
        memcpy( channel_raw_data, (*it).datax, length * sizeof(std::complex<float>));
        
    }
	else
	{
		assert(0 && L"Cannot find specified channel index");
		channel_raw_data = nullptr;
	}

    return channel_raw_data;
}

///
/// \brief RawData::MaxChanneIndex
/// \return
///
int RawData::MaxChannelIndex()
{
    int max_index = -1;
    for(const ChannelData& channel: _channels_data)
    {
        max_index = (max_index >= channel.channel_index) ? max_index : channel.channel_index;
    }
    return max_index;

}


const ChannelData& RawData::GetChannelInfo() const
{
    assert(_channels_data.size()>0);
    const ChannelData& result = _channels_data[0];

    //--
    int scan_id = _channels_data[0].scan_id;
    int phase_count = _channels_data[0].phase_count;
    for (auto iter = _channels_data.cbegin(); iter != _channels_data.cend(); iter++)
    {
        assert(scan_id ==(*iter).scan_id);
        assert(phase_count == (*iter).phase_count);

    }
    //-
    return result;
}

