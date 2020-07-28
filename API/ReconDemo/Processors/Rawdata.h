#ifndef RAWDATA_H
#define RAWDATA_H
#include <map>
#include <complex>
#include <vector>
#include <memory>

#include "QDataEvent.h"


///
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
	bool processed;//denote if there is new phase step data which can be used to reconstruct.
    std::complex<float>* datax;
    bool* phasemask_slices; //recording current phase mask of all slices.
    ChannelData():
        scan_id(0),
        channel_switch(0),
        channel_index(0),
        phase_index(0),
        freq_count(0),
        phase_count(0),
        slice_count(0),
        dim4(0),
		processed(true)
    {
        datax = nullptr;
        phasemask_slices = nullptr;
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
		processed = rhs.processed;
        int channel_size = freq_count*phase_count * slice_count;
		if (channel_size > 0) 
		{
			datax = new std::complex<float>[channel_size];
			memcpy(datax, rhs.datax, channel_size * sizeof(std::complex<float>));
		}
		if (phase_count*slice_count > 0)
		{
			phasemask_slices = new bool[phase_count*slice_count];
			memcpy(phasemask_slices, rhs.phasemask_slices, phase_count*slice_count * sizeof(bool));
		}
  
    }

    ChannelData& operator = (const ChannelData& rhs){
        scan_id = rhs.scan_id;
        channel_index = rhs.channel_index;
        phase_index = rhs.phase_index;
        freq_count = rhs.freq_count;
        phase_count = rhs.phase_count;
        slice_count = rhs.slice_count;
        dim4 = rhs.dim4;
		processed = rhs.processed;
        int channel_size = freq_count*phase_count * slice_count;
        if(datax!=nullptr)
        {
            delete[] datax;
        }
        else
        {
            datax= new std::complex<float>[channel_size];
        }
        memcpy(datax, rhs.datax, channel_size*sizeof(std::complex<float>));

        if(phasemask_slices!=nullptr)
        {
            delete[] phasemask_slices;
        }
        else
        {
            phasemask_slices = new bool[phase_count*slice_count];
        }
        memcpy(phasemask_slices, rhs.phasemask_slices, phase_count*slice_count*sizeof(bool));
        return *this;
    }
    ~ChannelData()
    {
        if(datax != nullptr)
            delete[] datax;
        if(phasemask_slices != nullptr)
            delete[] phasemask_slices;
    }


};
//
class RawData
{

public:
    static RawData& GetHandle() {return s_instance;}
    void Prepare(int scan_id, int freqcount, int phasecount, int slicecount, int channel_switch);
    bool Ready(){return _ready;}
    void InsertPhasedata(std::complex<float>* source, int length, int channel_index, int slice_index, int phase_index);
	int  MaxChannelIndex();
    const ChannelData& GetChannelInfo() const;

    std::complex<float>* GetChannelRawdata(int channel_index, int& freq_count, int& phase_count, int& slice_count);
	int GetNumofReadyphasesteps(int channel_index);//Used for controling number of reconstruction.
private:

    RawData(): _ready(false) {}

    bool InitChannels(int scan_id, int freqcount, int phasecount, int slicecount, int channel_switch);
	void UpdateChannel(ChannelData& channel_data, const complex<float>* data, const int freq_count, 
		const int channel_index, const int slice_index, const int phase_index);
	int  StateOfPhasesteps(const ChannelData& channel_data, int phase_index);
	void SetPhasestep(ChannelData& channel_data, int channel_index, int slice_index, int phase_index);
	vector<bool> GetReadyPhasesteps(const ChannelData& channel_data);


    static RawData s_instance;

    //An element of vector<ChannelData> stores all infomations and
    //data of a channel.
    //Note that the index of the vector element doesnot equal the channel index.
    std::vector<ChannelData> _channels_data;
    int _current_channel_index;
    int _current_slice_index;


    bool _ready;
};

}
#endif // RAWDATA_H
