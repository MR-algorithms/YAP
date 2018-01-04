#include "scantask.h"
#include "databin.h"



using namespace Scan;

ScanTask * ScantaskGenerator::Create(int trMs, Mask mask, std::wstring dataPath, int channelCount)
{
    ScanTask *scantask = new ScanTask;
    scantask->tr_millisecond = trMs;

    Databin databin;
    databin.Load(dataPath, channelCount);

    scantask->data = databin.GetRawData();

    int phaseCount = databin.GetPhaseCount();
    Mask::MaskType type = mask.type;
    float rate= mask.rate;


    scantask->mask = *(MaskGenerator::Create(phaseCount, type, rate).get());

    return scantask;

}
