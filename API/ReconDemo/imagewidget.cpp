#include "imagewidget.h"
#include "Client/DataHelper.h"
#include "Implement/DataObject.h"
#include "grayscalemapper.h"
#include <cassert>

#include <QPainter>
#include <algorithm>
#include <rectcalc.h>
#include <QMouseEvent>
#include <QApplication>
#include <QKeyEvent>
#include "imageobject.h"
#include "Implement/VariableSpace.h"
#include "windows.h"
#include <QDebug>

//#include "scanmanager.h"
//xhb->解耦scanmanager
using namespace Yap;
using namespace std;

ImageWidget::ImageWidget(QWidget *parent) : QWidget(parent),
    _data(YapShared<IData>(nullptr)),
    _properties(YapShared<IVariableContainer>(nullptr)),
    _zoom_mode(ZoomModeFit),
    _window_level(2048),
    _window_width(4096),
    _highlight(false),
    _image_size_ratio(1.0)
{

}

bool ImageWidget::SetImage(IData* data, bool repaint)
{
    _data = Yap::YapShared<IData>(data);


    if (data->GetVariables() != nullptr)
    {
        _properties = YapShared(data->GetVariables());
    }


    SetImageRect();
    if (data)
    {
        DataHelper image(_data.get());
        auto width = image.GetWidth();
        auto height = image.GetHeight();
        unsigned short * image_data = GetDataArray<unsigned short>(_data.get());
        auto max_value = max_element(image_data, image_data + width * height);

        _window_width = *max_value;
        _window_level = *max_value / 2;

       _mapper.SetWindowSize(size());
       _mapper.SetImageSize(QSize(int(width), int(height)));

       if (_properties.get() != nullptr)
       {

           VariableSpace variables(_properties.get());
           //auto fov_x = variables.Get<double>(L"FovRead");
           //auto fov_y = variables.Get<double>(L"FovPhase");

           auto fov_x = 100;
           auto fov_y = 100;

           _mapper.SetFOV(fov_x, fov_y);
           _mapper.UpdateMapper();
       }
    }

    if (repaint)
    {
        update();
    }

    return true;
}

void ImageWidget::Clear(bool repaint)
{
    _data.reset();
    _properties.reset();
    _mapper.Reset();
    if (repaint)
    {
        update();
    }
}

void ImageWidget::SetHighlight(bool highlight, bool repaint)
{
    _highlight = highlight;
    if (repaint)
    {
        update();
    }
}

bool ImageWidget::IsHighlighted() const
{
    return _highlight;
}

void ImageWidget::paintEvent(QPaintEvent * /*event*/)
{

    QPainter painter(this);

    QRect rect(QRect(0, 0, size().width(), size().height()));
    painter.fillRect(rect, QBrush(Qt::black));

    rect.adjust(2, 2, -2, -2);

    painter.setPen(QPen(_highlight ? QColor::fromRgb(215, 115, 115) : Qt::white));
    painter.drawRect(rect);

    if (_data)
    {

        //qDebug()<< "ImageWidget(this = "<< this <<")::Thread id = "<<GetCurrentThreadId();
        DataHelper image(_data.get());
        auto width = image.GetWidth();
        auto height = image.GetHeight();

        unsigned short * image_data = GetDataArray<unsigned short>(_data.get());

        unsigned char * buffer = new unsigned char[width * height];

        GrayScaleMapper mapper(_window_level, _window_width);

        for (unsigned int i = 0; i < width * height; ++i)
        {
            buffer[i] = mapper[image_data[i]];
        }

        rect.adjust(1, 1, -1, -1);
        QImage qimage(buffer, int(width), int(height), QImage::Format_Grayscale8);

        if (_zoom_mode == ZoomModeFit)
        {
            QRect target_rect = RectCalc::GetFitRect(rect, qimage.rect());
            painter.drawImage(target_rect, qimage, QRect(0, 0, int(width), int(height)));
        }
        else if (_zoom_mode == ZoomModeFill)
        {
            QRect image_rect = RectCalc::GetFillRect(rect, qimage.rect());
            painter.drawImage(rect, qimage, image_rect);
        }
        else
        {
            auto r_width = int(width * _image_size_ratio);
            auto r_height = int(height * _image_size_ratio);

            if (r_width < size().width() && r_height < size().height())
            {
                QRect target_rect{QRect(int((size().width() - r_width) / 2.0),
                                        int((size().height() - r_height) / 2.0),
                                        r_width, r_height)};
                painter.drawImage(target_rect, qimage, QRect(0, 0, int(width), int(height)));
            }
            else if (r_width < size().width() && r_height >= size().height())
            {
                QRect target_rect{QRect(int((size().width() - r_width) / 2.0), 3, r_width, size().height() - 6)};
                QRect image_rect{QRect(0, int((r_height - size().height()) / (r_height * 2.0) * height),
                                       width, int((double)size().height() / (double)r_height * height))};

                painter.drawImage(target_rect, qimage, image_rect);
            }
            else if (r_width >= size().width() && r_height < size().height())
            {
                QRect target_rect{QRect(3, int((size().height() - r_height) / 2.0), size().width() - 6, r_height)};
                QRect image_rect{QRect(int((r_width - size().width()) / (r_width * 2.0) * width), 0,
                                       int((double)size().width() / (double)r_width * width), height)};

                painter.drawImage(target_rect, qimage, image_rect);
            }
            else if (r_width >= size().width() && r_height >= size().height())
            {
                QRect image_rect{QRect(int((r_width - size().width()) / (r_width * 2.0) * width),
                                       int((r_height - size().height()) / (r_height * 2.0) * height),
                                       int((double)size().width() / (double)r_width * width),
                                       int((double)size().height() / (double)r_height * height))};
                painter.drawImage(rect, qimage, image_rect);
            }
        }

//        painter.drawText(rect, QString("Images info"));

    }
    else
    {
        _mapper.SetWindowSize(size());
        _mapper.SetImageSize(QSize(256, 256));

        if (_properties.get() != nullptr)
        {
            VariableSpace variables(_properties.get());
            auto fov_x = 100;//variables.Get<double>(L"FovRead");
            auto fov_y = 100;//variables.Get<double>(L"FovPhase");

            _mapper.SetFOV(fov_x, fov_y);
        }
        else
        {
            _mapper.SetFOV(80, 80);
        }

        _mapper.UpdateMapper();
    }

    DrawImageObjects(painter, _mapper);
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() | Qt::RightButton) == Qt::RightButton)
    {
        auto delta = event->pos() - _last_point;
        int delta_x = delta.x() * 20;
        int delta_y = delta.y() * 20;

        _last_point = event->pos();

        if (_window_width - int(delta_x) > 65535)
        {
            _window_width = 65535;
        }
        else if (_window_width - int(delta_x) < 255)
        {
            _window_width = 255;
        }
        else
        {
            _window_width -= delta_x;
        }

        if (_window_level + delta_y > 65535)
        {
            _window_level = 65535;
        }
        else if (_window_level + delta_y < 0)
        {
            _window_level = 0;
        }
        else
        {
            _window_level += delta_y;
        }

        update();
    }
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
    if (!_data)
        return;

    _last_point = event->pos();

    emit FocusSet(this);
}

void ImageWidget::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
        _zoom_mode = ZoomModeManual;
        _image_size_ratio += event->delta() / 1000.0;

        if (_image_size_ratio > 3.0)
        {
            _image_size_ratio = 3.0;
        }
        else if (_image_size_ratio < 0.5)
        {
            _image_size_ratio = 0.5;
        }

        update();
    }
}

void ImageWidget::ZoomFit()
{
    _zoom_mode = ZoomModeFit;
    update();
}

void ImageWidget::ZoomFill()
{
    _zoom_mode = ZoomModeFill;
    update();
}

void ImageWidget::AddImageObject(std::shared_ptr<ImageObject> object, bool reset_objects)
{
    if(reset_objects)
    {
        _image_objects.clear();
    }
    _image_objects.push_back(object);
}

void ImageWidget::DrawImageObjects(QPainter& painter, ImageWindowMapper& image_mapper)
{
    for(auto object : _image_objects)
    {
        object->Draw(painter, image_mapper);
    }
}

void ImageWidget::SetImageRect()
{
    if(!_data)
        return;

    if (_properties.get() != nullptr)
    {
        VariableSpace variables(_properties.get());
        auto cx = 100;//variables.Get<double>(L"FovRead");
        auto cy = 100;//variables.Get<double>(L"FovPhase");

        _image_rect.SetCx(cx);
        _image_rect.SetCy(cy);
        _image_rect.SetCenter(Point3D(0, 0, 0));
    }


    auto data_geometry = _data->GetGeometry();
    if (data_geometry == nullptr)
        return;

    double x, y, z;
    data_geometry->GetRowVector(x, y, z);
    Vector3D row(x, y, z);
    data_geometry->GetColumnVector(x, y, z);
    Vector3D column(x, y, z);

    _image_rect.SetOrientation(row, column);

    //if (image.GetPhaseEncodingDirection() == PhaseEncodingInRows)
    //    {
   //         rect.SetCx(image.GetFov().cy);
    //        rect.SetCy(image.GetFov().cx);
  //      }
    //    else
    //    {
   //         rect.SetCx(image.GetFov().cx);
    //        rect.SetCy(image.GetFov().cy);
   //     }
   //     rect.SetOrientation(image.GetImageOrientation().row, image.GetImageOrientation().column);
  //      rect.SetCenter(image.GetCenter(image));

}

std::pair<bool, Geometry::CRect3D> ImageWidget::GetImageRect()
{
    return std::make_pair(_data ? true : false, _image_rect);
}

void ImageWidget::SetVector(double x1, double y1, double z1, double x2, double y2, double z2)
{
    //load涓€涓枃浠剁殑鏃跺€欙紝鐢变簬鐩墠鏂囦欢淇濆瓨鐨勫弬鏁伴儴鍒嗘病鍋氬ソ锛屾墍浠ユ殏鏃跺厛纭紪鐮佷竴涓畾浣嶄俊鎭?
    Vector3D row(x1, y1, z1);
    Vector3D column(x2, y2, z2);
    _image_rect.SetOrientation(row, column);
}
