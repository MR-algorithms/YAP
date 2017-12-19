#ifndef CMASK_H
#define CMASK_H
#include <vector>
#include <memory>

enum ScanMode
{
    smFullSampling,
    smEqualSampling,// 你好
    smCustom
};

struct Mask
{
    std::vector<int> data;
    unsigned int currentIndex;

    float sampleRate;
};
class MaskGenerator
{
public:
    MaskGenerator();
    std::shared_ptr<Mask> Create(unsigned int phaseCount, ScanMode mode, float sampleRate);


};

#endif // CMASK_H
