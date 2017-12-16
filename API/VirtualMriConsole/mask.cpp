#include "mask.h"
#include <cassert>

Mask::Mask()
{

}

bool Mask::Initialize(int phaseCount, ScanMode mode, float sampleRate)
{
    switch(mode)
    {
    case smFullSampling:
    {
        _data.resize(phaseCount);
        for(unsigned i = 0; i < phaseCount; i ++)
        {
            _data[i] = i;
        }
    }
        break;
    case smEqualSampling:
    {
        int sampleStep = static_cast<int>( floor(1.0 / (sampleRate - 0.01) + 0.5) );
        for(unsigned i = 0; i < phaseCount; i ++)
        {
            int index = i % sampleStep; //每一组第几个；
            if( index == 0)
            {
                _data.push_back(i);
            }
        }

        assert(_data.size()*sampleStep <= phaseCount);

    }
        break;
    default:
        break;
    }
    return true;
}
