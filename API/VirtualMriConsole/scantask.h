#ifndef SCANTASK_H
#define SCANTASK_H
#include <vector>
#include <complex>
#include "mask.h"
#include <boost/shared_array.hpp>
using namespace std;

namespace Scan
{
    struct ScanTask
    {
        Mask mask;
        boost::shared_array<complex<float>> data;
        int tr_millisecond;
        ScanTask():data(nullptr),tr_millisecond(-1){}
    };

    class ScantaskGenerator
    {
    public:
        ScantaskGenerator(){}
        static ScanTask * Create(int trMs, Mask mask, std::wstring dataPath);

    };

}

#endif // SCANTASK_H
