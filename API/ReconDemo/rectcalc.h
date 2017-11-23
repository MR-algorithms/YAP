#ifndef RECTCALC_H
#define RECTCALC_H
#include <QRect>
#include <QSize>

namespace RectCalc
{
    QRect GetFillRect(const QRect& window_rect, const QRect& image_rect);
    QRect GetFitRect(const QRect& window_rect, const QRect& image_rect);
    QSize GetFitSize(const QSize& window_size, const QSize& image_size);
    QSize GetFillSize(const QSize& window_size, const QSize& image_size);
};

#endif // RECTCALC_H
