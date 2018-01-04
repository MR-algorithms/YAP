#include "scantask.h"
#include "databin.h"



using namespace Scan;

ScanTask * ScantaskGenerator::Create(int trMs, Mask mask, std::wstring dataPath)
{
    ScanTask *scantask = new ScanTask;

    Databin databin;
    databin.Load(dataPath);

    scantask->data = databin.GetRawData();

    int phaseCount = databin.GetPhaseCount();
    Mask::MaskType mt = mask.maskType;
    int sampleRate= mask.sampleRate;


    MaskGenerator::Create(phaseCount, mt, sampleRate);

    return scantask;

}
