#include "scantask.h"



using namespace Scan;

ScanTask ScantaskGenerator::Create(ScanTask ref_scantask, Mask ref_mask)
{
    ScanTask scantask;

    scantask.trMs = ref_scantask.trMs;
    scantask.dataPath = ref_scantask.dataPath;
    scantask.ip_address = ref_scantask.ip_address;
    scantask.port = ref_scantask.port;

    Mask::MaskType type = ref_mask.type;
    float rate= ref_mask.rate;
    unsigned int phaseCount = ref_mask.phaseCount;
    unsigned int channelCount = ref_mask.channelCount;

    scantask.mask = MaskGenerator::Create(channelCount, phaseCount, type, rate);

    return scantask;

}
