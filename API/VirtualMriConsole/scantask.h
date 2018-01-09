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
        std::wstring dataPath;
        std::wstring ip_address;
        int port;
        int trMs;
        ScanTask():trMs(-1), dataPath(L""), port(-1), ip_address(L""){}

        ScanTask& operator = (const ScanTask& rhs){
            mask = rhs.mask;
            dataPath = rhs.dataPath;
            ip_address = rhs.ip_address;
            port = rhs.port;
            trMs = rhs.trMs;

            return *this;
        }
    };

    class ScantaskGenerator
    {
    public:
        ScantaskGenerator(){}
        static ScanTask Create(ScanTask ref_scantask, Mask ref_mask);

    };

}

#endif // SCANTASK_H
