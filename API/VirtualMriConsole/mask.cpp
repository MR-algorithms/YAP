#include "mask.h"
#include <cassert>
#include <memory>
#include <fstream>
using namespace Scan;

Mask MaskGenerator::Create(unsigned int channelCount, unsigned int phaseCount, Mask::MaskType masktype, float sampleRate)
{
    Mask mask;
    mask.rate = sampleRate;
    mask.type = masktype;
    mask.channelCount = channelCount;
    mask.phaseCount = phaseCount;
    int toSendPhaseCount=phaseCount*sampleRate;

    switch(masktype)
    {
    case Mask::mtSequential:
    {
        mask.data.resize(toSendPhaseCount);
        for(unsigned i = 0; i < toSendPhaseCount; i ++)
        {
            mask.data[i] = i;
        }
    }
        break;
    case Mask::mtFromCenter:
    {
        assert(phaseCount % 2 == 0);
        mask.data.resize(toSendPhaseCount);
        unsigned int centerIndex = phaseCount / 2;
        int temp=toSendPhaseCount/2;

        for(unsigned i = 0; i <temp ; i ++)
        {
            mask.data[2 * i] = centerIndex - 1 - i;
            mask.data[2 * i + 1] = centerIndex + i;
        }
        if(toSendPhaseCount%2!=0)
        {
            mask.data[2*temp]=centerIndex-1-temp;
        }

    }
        break;
    case Mask::mtFromSide:
    {
        assert(phaseCount % 2 == 0);
        mask.data.resize(toSendPhaseCount);
        unsigned int centerIndex = phaseCount / 2;
        int temp=toSendPhaseCount/2;

        for(unsigned i = 0; i < temp; i ++)
        {
            mask.data[2 * i] = i;
            mask.data[2 * i + 1] = phaseCount - 1 - i;
        }
        if(toSendPhaseCount%2!=0)
        {
            mask.data[2*temp]=temp;
        }

    }
        break;
    case Mask::mtEqualSampling:
    {
        mask.data.resize(toSendPhaseCount);
        int sampleStep = static_cast<int>( floor(1.0 / (sampleRate - 0.01) + 0.5) );
        unsigned int groupIndex = 0;
        for(unsigned i = 0; i < phaseCount; i ++)
        {
            int index = i % sampleStep; //每一组第几个；
            if(( index == 0)&&(groupIndex < toSendPhaseCount))
            {
                mask.data[groupIndex] = i;
                groupIndex ++;
            }
        }

        //Keep the same size for consistency.
        /* for(unsigned int i = groupIndex; i < phaseCount; i ++)
        {
            mask.data[i] = 999;
        }
        */
        assert(mask.data.size()*sampleStep <= phaseCount);

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
