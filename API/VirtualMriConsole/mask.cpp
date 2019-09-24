#include "mask.h"
#include <cassert>
#include <memory>
#include <fstream>
using namespace Scan;

Mask MaskGenerator::Create(unsigned int channelCount, unsigned int phaseCount, Mask::MaskType masktype, float sampleRate)
{
    Mask mask;
    assert(sampleRate >0 && sampleRate <= 1.0);
    mask.rate = sampleRate;
    mask.type = masktype;
    mask.channelCount = channelCount;
    mask.phaseCount = phaseCount;

    switch(masktype)
    {
    case Mask::mtSequential:
    {
        int masksize = sampleRate * phaseCount;
        mask.data.resize(masksize);

        for(unsigned i = 0; i < masksize; i ++)
        {
            mask.data[i] = i;
        }
    }
        break;
    case Mask::mtFromCenter:
    {
        int masksize = sampleRate *  phaseCount;
        if(masksize%2 != 0)
        {
            masksize ++;
        }
        assert(phaseCount % 2 == 0);
        unsigned int centerIndex = phaseCount / 2;

        int i = 0;

        while(mask.data.size() < masksize)
        {
            mask.data.push_back( centerIndex - 1 - i );
            mask.data.push_back( centerIndex + i );
            i ++;
        }
        int x =  100 ;

    }
        break;
        /*
    case Mask::mtFromCenter:
    {
        //Dont support sample rate other than 1.0;
        assert(sampleRate == 1.0);
        assert(phaseCount % 2 == 0);
        mask.data.resize(phaseCount);
        unsigned int centerIndex = phaseCount / 2;

        for(unsigned i = 0; i < centerIndex; i ++)
        {
            mask.data[2 * i] = centerIndex - 1 - i;
            mask.data[2 * i + 1] = centerIndex + i;
        }

    }
        break;
        */
    case Mask::mtFromSide:
    {
        //Dont support sample rate other than 1.0;
        assert(sampleRate == 1.0);

        assert(phaseCount % 2 == 0);
        mask.data.resize(phaseCount);
        unsigned int centerIndex = phaseCount / 2;

        for(unsigned i = 0; i < centerIndex; i ++)
        {
            mask.data[2 * i] = i;
            mask.data[2 * i + 1] = phaseCount - 1 - i;
        }

    }
        break;
    case Mask::mtEqualSampling:
    {
        mask.data.resize(phaseCount);
        int sampleStep = static_cast<int>( floor(1.0 / (sampleRate - 0.01) + 0.5) );
        unsigned int groupIndex = 0;
        for(unsigned i = 0; i < phaseCount; i ++)
        {
            int index = i % sampleStep; //每一组第几个；
            if( index == 0)
            {
                mask.data[groupIndex] = i;
                groupIndex ++;
            }
        }

        //Keep the same size for consistency.
        for(unsigned int i = groupIndex; i < phaseCount; i ++)
        {
            mask.data[i] = 999;
        }
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
