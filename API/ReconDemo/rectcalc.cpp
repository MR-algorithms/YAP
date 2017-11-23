#include "rectcalc.h"


// 此函数返回Fill模式下显示在窗口中的图像区域。
QRect RectCalc::GetFillRect(const QRect& window_rect, const QRect& image_rect)
{
    return GetFitRect(image_rect, window_rect);
}

// 此函数返回Fit模式下用于显示图像的窗口区域。
QRect RectCalc::GetFitRect(const QRect& window_rect, const QRect& image_rect)
{
    QRect rect = window_rect;
    if (image_rect.width() / double(image_rect.height()) < window_rect.width() / (double)window_rect.height())
    {
        // 图像比较窄
        int rect_width = window_rect.height() / double(image_rect.height()) * image_rect.width();
        rect.setLeft((window_rect.width() - rect_width) / 2);
        rect.setRight((window_rect.width() + rect_width) / 2);
    }
    else // 图像比较宽
    {
        int rect_height = window_rect.width() / double(image_rect.width()) * image_rect.height();
        rect.setTop((window_rect.height() - rect_height) / 2);
        rect.setBottom((window_rect.height() + rect_height) / 2);
    }

    return rect;
}

QSize RectCalc::GetFitSize(const QSize& window_size, const QSize& image_size)
{
    QRect window_rect(0, 0, window_size.width(), window_size.height());
    QRect image_rect(0, 0, image_size.width(), image_size.height());
    QRect fit_rect = GetFitRect(window_rect, image_rect);
    return QSize(fit_rect.width(), fit_rect.height());
}

QSize RectCalc::GetFillSize(const QSize& window_size, const QSize& image_size)
{
    QRect window_rect(0, 0, window_size.width(), window_size.height());
    QRect image_rect(0, 0, image_size.width(), image_size.height());
    QRect fill_rect = GetFillRect(window_rect, image_rect);
    return QSize(fill_rect.width(), fill_rect.height());
}
