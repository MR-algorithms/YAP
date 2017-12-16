#ifndef DATABIN_H
#define DATABIN_H
#include "mask.h"
#include <boost/shared_array.hpp>
#include <complex>

using namespace std;

class Databin
{
private:
    //wchar_t* _dataPath;
    Mask _mask;
    boost::shared_array<complex<float>> _data;

public:
    Databin();
    void Load(std::wstring dataPath);
};

#endif // DATABIN_H
