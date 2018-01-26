#ifndef DATABIN_H
#define DATABIN_H
#include <boost/shared_array.hpp>
#include <complex>
#include "RawDataReader.h"
//#include "communicator.h"
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

    unsigned int GetPhaseCount();
    int _current_phase_index;
    std::shared_ptr<Communicator> _communicator;
    bool _ended;

public:

    Databin();

    std::shared_ptr<Communicator> GetCommunicator(){return _communicator;}
    void Load(std::wstring dataPath, int channelCount);
    void Start(int scan_id, int channel_count);
    void Go();
    void End();
    bool CanbeFinished(){return _current_phase_index >= static_cast<int>( GetPhaseCount());}
    bool Stoped(){return _ended;}



};

#endif // DATABIN_H
