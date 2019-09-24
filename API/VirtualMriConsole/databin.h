#ifndef DATABIN_H
#define DATABIN_H
#include <boost/shared_array.hpp>
#include <complex>
#include "RawDataReader.h"
//#include "communicator.h"
#include "SampleDataProtocol.h"
#include "scantask.h"

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

    void Load(std::wstring dataPath, int channelCount);
    unsigned int GetPhaseCount();
    int _current_scanmask_index;
    std::shared_ptr<Communicator> _communicator;
    bool _ended;
    SampleDataStart _start;
    Scan::ScanTask _scantask;


public:

    Databin(const Scan::ScanTask & scantask);
    std::shared_ptr<Communicator> GetCommunicator(){return _communicator;}
    void Load(){Load(_scantask.dataPath, _scantask.mask.channelCount);}
    void Start(int scan_id);
    bool Go();
    void End();
    bool CanbeFinished(){return _current_scanmask_index >=static_cast<int>( _scantask.mask.data.size() );}
    bool Stoped(){return _ended;}



};

#endif // DATABIN_H
