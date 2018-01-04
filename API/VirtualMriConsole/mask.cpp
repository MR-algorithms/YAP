#include "mask.h"
#include <cassert>
#include <memory>
#include <fstream>
using namespace Scan;

std::shared_ptr<Mask> MaskGenerator::Create(unsigned int phaseCount, Mask::MaskType masktype, float sampleRate)
{
    std::shared_ptr<Mask> mask(new Mask);
    switch(masktype)
    {
    case Mask::mtSequential:
    {
        mask.get()->data.resize(phaseCount);
        for(unsigned i = 0; i < phaseCount; i ++)
        {
            mask.get()->data[i] = i;
        }
    }
        break;
    case Mask::mtFromCenter:
    {
        assert(phaseCount % 2 == 0);
        mask.get()->data.resize(phaseCount);
        unsigned int centerIndex = phaseCount / 2;

        for(unsigned i = 0; i < centerIndex; i ++)
        {
            mask.get()->data[2 * i] = centerIndex - 1 - i;
            mask.get()->data[2 * i + 1] = centerIndex + i;
        }

    }
        break;
    case Mask::mtFromSide:
    {
        assert(phaseCount % 2 == 0);
        mask.get()->data.resize(phaseCount);
        unsigned int centerIndex = phaseCount / 2;

        for(unsigned i = 0; i < centerIndex; i ++)
        {
            mask.get()->data[2 * i] = i;
            mask.get()->data[2 * i + 1] = phaseCount - 1 - i;
        }

    }
        break;
    case Mask::mtEqualSampling:
    {
        mask.get()->data.resize(phaseCount);
        int sampleStep = static_cast<int>( floor(1.0 / (sampleRate - 0.01) + 0.5) );
        unsigned int groupIndex = 0;
        for(unsigned i = 0; i < phaseCount; i ++)
        {
            int index = i % sampleStep; //每一组第几个；
            if( index == 0)
            {
                mask.get()->data[groupIndex] = i;
                groupIndex ++;
            }
        }

        //Keep the same size for consistency.
        for(unsigned int i = groupIndex; i < phaseCount; i ++)
        {
            mask.get()->data[i] = 999;
        }
        assert(mask.get()->data.size()*sampleStep <= phaseCount);

    }
        break;
    default:
        break;
    }
    return mask;
}


bool MaskGenerator::Write(std::wstring path)
{
   // std::fstream file()
    return false;
}
