#ifndef DATABIN_H
#define DATABIN_H
#include "mask.h"
#include <boost/shared_array.hpp>
#include <complex>
struct RawDataInfo;

using namespace std;

class Databin
{
private:
    //wchar_t* _dataPath;
    Mask _mask;
    boost::shared_array<complex<float>> _data;
    std::shared_ptr<RawDataInfo> _dataInfo;

public:

    Databin();

    void Load(std::wstring dataPath);

    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex);

    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex, unsigned int sliceIndex);

    boost::shared_array<complex<float>> GetRawData(unsigned int channelIndex, unsigned int sliceIndex, unsigned int phaseIndex);

};

#endif // DATABIN_H
