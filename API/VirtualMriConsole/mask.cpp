#include "mask.h"
#include <cassert>
#include <memory>

std::shared_ptr<Mask> MaskGenerator::Create(unsigned int phaseCount, ScanMode mode, float sampleRate)
{
    std::shared_ptr<Mask> mask(new Mask);
    switch(mode)
    {
    case smFullSampling:
    {
        mask.get()->data.resize(phaseCount);
        for(unsigned i = 0; i < phaseCount; i ++)
        {
            mask.get()->data[i] = i;
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
                mask.get()->data.push_back(i);
            }
        }

        assert(mask.get()->data.size()*sampleStep <= phaseCount);

    }
        break;
    default:
        break;
    }
    return mask;
}
