#ifndef DATABIN_H
#define DATABIN_H
#include <boost/shared_array.hpp>
#include <complex>
struct RawDataInfo;

using namespace std;

class Databin
{
private:
    boost::shared_array<complex<float>> _data;
    std::shared_ptr<RawDataInfo> _dataInfo;
    unsigned int AllChannel(int channelCount);

public:

    Databin();

    void Load(std::wstring dataPath, int channelCount);

    boost::shared_array<complex<float>> GetRawData();
    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex);
    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex, unsigned int sliceIndex);
    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex, unsigned int sliceIndex, unsigned int phaseIndex);

    unsigned int GetPhaseCount();

};

#endif // DATABIN_H
