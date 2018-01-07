#ifndef GRAYSCALEMAPPER_H
#define GRAYSCALEMAPPER_H

#include <vector>

class GrayScaleMapper
{
public:
    GrayScaleMapper(unsigned short level, unsigned short width);

    void SetLevelWidth(unsigned short level, unsigned short width);

    unsigned char operator [](unsigned short value)
    {
        return _lut[value];
    }

private:
    unsigned short _level;
    unsigned short _width;

    std::vector<unsigned char> _lut;

    void Update();
};

#endif // GRAYSCALEMAPPER_H
