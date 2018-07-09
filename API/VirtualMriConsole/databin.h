#ifndef DATABIN_H
#define DATABIN_H
#include <boost/shared_array.hpp>
#include <complex>
#include "RawDataReader.h"
//#include "communicator.h"
#include "SampleDataProtocol.h"

class Communicator;
using namespace std;

class Databin
{
private:
    boost::shared_array<complex<float>> _data;
    RawDataInfo _dataInfo;
    unsigned int AllChannel(int channelCount);

    boost::shared_array<complex<float>> GetRawData();
    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex);
    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex, unsigned int sliceIndex);
    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex, unsigned int sliceIndex, unsigned int phaseIndex);

    //unsigned int GetPhaseCount();
    int _current_phase_index;
    std::shared_ptr<Communicator> _communicator;
    bool _ended;
    SampleDataStart _start;

    int _to_send_phase_count=65536;

public:

    Databin();
    unsigned int GetPhaseCount();
    void SetCurrentPhaseIndex(const int& currentPhaseIndex){_current_phase_index=currentPhaseIndex;}
    void SetSendPhaseCount(const int& toSendPhaseCount){_to_send_phase_count=toSendPhaseCount;}

    std::shared_ptr<Communicator> GetCommunicator(){return _communicator;}
    void Load(std::wstring dataPath, int channelCount);
    void Start(int scan_id, int channel_count);
    bool Go(const vector<int>& mask_vector);
    void End();
    //bool CanbeFinished(){return _current_phase_index >=static_cast<int>( GetPhaseCount());}
    bool CanbeFinished(){return _current_phase_index >=_to_send_phase_count;}
    bool Stoped(){return _ended;}



};

#endif // DATABIN_H
