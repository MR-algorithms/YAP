#include "scantask.h"



using namespace Scan;

ScanTask ScantaskGenerator::Create(int trMs, Mask reference, std::wstring dataPath)
{
    ScanTask scantask;

    scantask.trMs = trMs;
    scantask.dataPath = dataPath;



    Mask::MaskType type = reference.type;
    float rate= reference.rate;
    unsigned int phaseCount = reference.phaseCount;
    unsigned int channelCount = reference.channelCount;

    scantask.mask = MaskGenerator::Create(channelCount, phaseCount, type, rate);

    return scantask;

}
