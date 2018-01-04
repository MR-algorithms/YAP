#ifndef CMASK_H
#define CMASK_H
#include <vector>
#include <memory>

namespace Scan
{
    struct Mask
    {
        enum MaskType
        {
            mtSequential,
            mtFromCenter,
            mtFromSide,
            mtEqualSampling,
            mtCustom,
            mtNone
        };

        std::vector<int> data;

        float sampleRate;
        MaskType maskType;
        Mask(): sampleRate(0), maskType(mtNone){ data.resize(0);}
        Mask(float rate, MaskType type): sampleRate(rate), maskType(type){ data.resize(0);}
    };
    class MaskGenerator
    {
    public:
        MaskGenerator();
        static std::shared_ptr<Mask> Create(unsigned int phaseCount, Mask::MaskType masktype, float sampleRate);

        static bool Write(std::wstring path);

    };
}
#endif // CMASK_H
