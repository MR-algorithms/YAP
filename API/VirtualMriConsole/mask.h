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
        Mask(): rate(0), type(mtNone){ data.resize(0);}
        Mask(float rate, MaskType type): rate(rate), type(type){ data.resize(0);}
        Mask& operator = (const Mask& rhs){
            rate = rhs.rate;
            type = rhs.type;
            data.assign(rhs.data.begin(),rhs.data.end());
            return *this;
        }
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
