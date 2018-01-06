#ifndef DATABIN_H
#define DATABIN_H
#include <boost/shared_array.hpp>
#include <complex>
#include "RawDataReader.h"
using namespace std;

class Databin
{
private:
    boost::shared_array<complex<float>> _data;
    RawDataInfo _dataInfo;
    unsigned int AllChannel(int channelCount);
    int _currentIndex;//

    boost::shared_array<complex<float>> GetRawData();
    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex);
    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex, unsigned int sliceIndex);
    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex, unsigned int sliceIndex, unsigned int phaseIndex);

    unsigned int GetPhaseCount();
    bool CanbeFinished(){return false;}
    bool _ended;

public:

    Databin():_ended(false){}

    void Load(std::wstring dataPath, int channelCount);
    void Start(int scan_id, int channel_count);
    void Go();
    void End();



};

#endif // DATABIN_H
