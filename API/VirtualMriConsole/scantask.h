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
        int trMs;
        ScanTask():trMs(-1), dataPath(L""){}

        ScanTask& operator = (const ScanTask& rhs){
            mask = rhs.mask;
            dataPath = rhs.dataPath;
            trMs = rhs.trMs;

            return *this;
        }
    };

    class ScantaskGenerator
    {
    public:
        ScantaskGenerator(){}
        static ScanTask Create(int trMs, Mask reference, std::wstring dataPath);

    };

}

#endif // SCANTASK_H
