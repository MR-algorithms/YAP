#include "grayscalemapper.h"

GrayScaleMapper::GrayScaleMapper(unsigned short level, unsigned short width):
    _level(level),
    _width(width)
{
    Update();
}

void GrayScaleMapper::SetLevelWidth(unsigned short level, unsigned short width)
{
    if (level == _level && width == _width)
        return;

    _level = level;
    _width = width;

    Update();
}

void GrayScaleMapper::Update()//根据窗宽窗位将灰度值映射到0~255之间
{
    _lut.resize(65536);

    double a = 255.0 / _width;
    double b = -255.0 * (_level - _width / 2.0) / _width;

    for (unsigned short i = 0; i < 65535; ++i)
    {
        double result = i * a + b;
        if (result < 0)
        {
            result = 0;
        }
        else if (result > 255.0)
        {
            result = 255.0;
        }
        _lut[i] = static_cast<unsigned char>(result + 0.5); // 四舍五入
    }
}
