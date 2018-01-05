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

        float rate;
        MaskType type;
        int phaseCount;
        int channelCount;

        Mask(): rate(0), type(mtNone),phaseCount(0), channelCount(0){ data.resize(0);}
        Mask(float rate, MaskType type, int phases, int channels ):
            rate(rate), type(type),phaseCount(phases), channelCount(channels) { data.resize(0);}
        Mask& operator = (const Mask& rhs){
            rate = rhs.rate;
            type = rhs.type;
            phaseCount = rhs.phaseCount;
            channelCount = rhs.channelCount;

            data.assign(rhs.data.begin(),rhs.data.end());
            return *this;
        }
    };
    class MaskGenerator
    {
    public:
        MaskGenerator();
        static Mask Create(unsigned int channelCount, unsigned int phaseCount, Mask::MaskType masktype, float sampleRate);

        static bool Write(std::wstring path);

    };
}
#endif // CMASK_H
