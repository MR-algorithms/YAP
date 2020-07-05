#include "imagelayoutwidget.h"
#include "imagewidget.h"
#include "assert.h"

#include <QMouseEvent>
#include <QApplication>
#include <QKeyEvent>
#include <qDebug>
#include "Client/DataHelper.h"

//#include "Processors/Display2D.h"



using namespace std;
using namespace Yap;

ImageLayoutWidget::ImageLayoutWidget(QWidget *parent) :
    QWidget(parent),
    _layout(4,4),
    _max_layout(8,4),
    _first_visible_image_index(0),
    _current_image_index(0)
{
    SetLayout(_layout);
}

bool ImageLayoutWidget::AddImage(IData* data )
{
    if (data == nullptr)
        return false;

    _images.push_back(YapShared<IData>(data));

    auto images_size = _images.size();
    assert(images_size - 1 >= _first_visible_image_index);


    auto widget_index=images_size - 1 - _first_visible_image_index;


    if (widget_index < _image_widgets.size())
    {
        _image_widgets[widget_index]->SetImage(data);
        //qDebug()<<"ImageWidget::update/repaint()";
        _image_widgets[widget_index]->repaint();
        //_image_widgets[widget_index]->update();

    }

    HighlightCurrentWidget();
    return true;
}
///
/// \brief ImageLayoutWidget::UpdateImage
/// You should
/// \param data
/// \param image_key
/// \return
///
bool ImageLayoutWidget::UpdateImage(Yap::IData* data, unsigned int image_key)
{
    if (data == nullptr)
        return false;

    if(image_key >= _images.size())
    {
        AddImage(data);
    }
    else
    {
        //What about the original element?
        _images[image_key] = YapShared<IData>(data);

        auto widget_index = image_key - _first_visible_image_index;

        if (widget_index < _image_widgets.size())
        {
            _image_widgets[widget_index]->SetImage(data);
            //qDebug()<<"ImageWidget::update/repaint()";
            _image_widgets[widget_index]->repaint();
            //_image_widgets[widget_index]->update();


        }
    }


    return true;
}

const std::vector<Yap::SmartPtr<IData>> ImageLayoutWidget::GetImages() const
{
    return _images;
}

void ImageLayoutWidget::ClearImages()
{
    for (auto image : _images)
    {
        _images_erased.emplace_back(image);
    }

    _images.clear();
    _current_image_index = 0;

    UpdateAll();
}

void ImageLayoutWidget::ClearSingleImage()
{
    if (!_images.empty())
    {
        _images_erased.emplace_back(_images[_current_image_index]);
        _images.erase(_images.begin() + _current_image_index);

        if (_current_image_index >= _images.size())
        {
            if (_images.size() == 0)
            {
                _current_image_index = 0;
            }
            else
            {
                _current_image_index = _images.size() - 1;
            }

        }

        UpdateAll();
    }

}

void ImageLayoutWidget::BackupImage()
{
    if (!_images_erased.empty())
    {
        _images.emplace_back(_images_erased[0]);
        _images_erased.erase(_images_erased.begin());

        UpdateAll();
    }
}

void ImageLayoutWidget::UpdateImageWidgets()
{
    for (int i = 0; i < _layout.width() * _layout.height(); ++i)
    {
        if (_first_visible_image_index + i < _images.size())
        {
            _image_widgets[i]->SetImage(_images[_first_visible_image_index + i].get(), true);
        }
        else
        {
            _image_widgets[i]->Clear();
        }
    }
}

void ImageLayoutWidget::UpdateAll()
{
    int width = this->size().width() / _layout.width();
    int height = this->size().height() / _layout.height();

    for (int row = 0; row < _layout.height(); ++row)
    {
        for (int column = 0; column < _layout.width(); ++column)
        {

            _image_widgets[row * _layout.width() + column]->setGeometry(
                        column * width, row * height, width, height);
        }
    }

    _first_visible_image_index = MakeImageVisible(_current_image_index);

    UpdateImageWidgets();

    HighlightCurrentWidget();

    for (int i = 0; i < _layout.width() * _layout.height(); ++i)
    {
        _image_widgets[i]->show();
        _image_widgets[i]->update();
    }
    for (unsigned int i = _layout.width() * _layout.height(); i < _image_widgets.size(); ++i)
    {
        _image_widgets[i]->hide();
    }
}

unsigned int ImageLayoutWidget::MakeImageVisible(unsigned int image_index) const
{
    {
        //unsigned int first_visible_index = 7 / _layout.width() * _layout.width();
        //auto last = (17 - 1) / _layout.width() * _layout.width();
        //auto max_first_visible_index = last - (_layout.height() - 1) * _layout.width();
        //first_visible_index = unsigned int((first_visible_index < max_first_visible_index)
        //                                   ? first_visible_index : max_first_visible_index);
        //int x = 1;
    }
    //
    if (_images.empty())
        return 0;

    assert(image_index < _images.size());

    unsigned int first_visible_index = 0;


    if (_images.size() > (unsigned int)(_layout.width() * _layout.height()))
    {
        first_visible_index = image_index / _layout.width() * _layout.width();
        auto last = (_images.size() - 1) / _layout.width() * _layout.width();
        auto max_first_visible_index = last - (_layout.height() - 1) * _layout.width();
        first_visible_index = unsigned int((first_visible_index < max_first_visible_index)
                                           ? first_visible_index : max_first_visible_index);
    }

    return first_visible_index;
}

bool ImageLayoutWidget::SetLayout(QSize layout)
{
    if (layout.width() > _max_layout.width() || layout.height() > _max_layout.height())
        return false;

    _layout = layout;

    while (_image_widgets.size() < (unsigned int)(_layout.width() * _layout.height()))
    {
        auto image_widget = new ImageWidget(this);
        _image_widgets.push_back(image_widget);
        connect(image_widget, &ImageWidget::FocusSet, this, &ImageLayoutWidget::OnFocusChange);
    }

    _first_visible_image_index = MakeImageVisible(_current_image_index);

    UpdateAll();

    return true;
}

QSize ImageLayoutWidget::GetLayout() const
{
    return _layout;
}

void ImageLayoutWidget::ZoomFit()
{
    for (auto widget : _image_widgets)
    {
        widget->ZoomFit();
    }
}

void ImageLayoutWidget::ZoomFill()
{
    for (auto widget : _image_widgets)
    {
        widget->ZoomFill();
    }
}

ImageWidget *ImageLayoutWidget::GetImageWidget(unsigned int image_index)
{
    assert(!_images.empty() && image_index < _images.size());
    assert((image_index - _first_visible_image_index) >= 0);
    assert(int(image_index - _first_visible_image_index) < _layout.width() * _layout.height());

    return _image_widgets[image_index - _first_visible_image_index];
}

void ImageLayoutWidget::resizeEvent(QResizeEvent *event)
{
    assert(event != nullptr);
    UpdateAll();
}

void ImageLayoutWidget::wheelEvent(QWheelEvent *event)
{
   if (!_images.empty())
    {
       if (QApplication::keyboardModifiers() == Qt::NoModifier)
       {
           if (event->delta()<0) //scroll down
           {
               LineDown();
           }
           else
           {
               LineUp();
           }
       }

   }
}

void ImageLayoutWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_PageUp:
        PageUp();
        break;
    case Qt::Key_PageDown:
        PageDown();
        break;
    case Qt::Key_Home:
        Home();
        break;
    case Qt::Key_End:
        End();
        break;
    case Qt::Key_Down:
        LineDown();
        break;
    case Qt::Key_Up:
        LineUp();
        break;
    case Qt::Key_Left:
        Left();
        break;
    case Qt::Key_Right:
        Right();
        break;
    default:
        QWidget::keyPressEvent(event);
    }

}

void ImageLayoutWidget::HighlightCurrentWidget()
{
    if (_images.empty())
        return;

    assert(_current_image_index < _images.size());

    for (auto image_widget : _image_widgets)
    {
        image_widget->SetHighlight(image_widget == GetImageWidget(_current_image_index), true);
    }
}

void ImageLayoutWidget::PageDown()
{
    assert(_images.empty() || _current_image_index < _images.size());

    if(_current_image_index + _layout.width() * _layout.height() < _images.size())
    {
        _current_image_index += _layout.width() * _layout.height();
    }
    else
    {
        _current_image_index = unsigned int(_images.size() - 1);
    }
    _first_visible_image_index =
            _current_image_index /
            (_layout.width() * _layout.height()) *
            (_layout.width() * _layout.height());

    UpdateImageWidgets();

    HighlightCurrentWidget();
}

void ImageLayoutWidget::PageUp()
{
    assert(_images.empty() || _current_image_index < _images.size());

    if(int(_current_image_index) < _layout.width() * _layout.height())
    {
        _current_image_index = 0;
    }
    else
    {
        _current_image_index -= _layout.width() * _layout.height();
    }

    _first_visible_image_index =
            _current_image_index /
            (_layout.width() * _layout.height()) *
            (_layout.width() * _layout.height());

    UpdateImageWidgets();
    HighlightCurrentWidget();
}

void ImageLayoutWidget::Left()
{
    assert(_images.empty() || _current_image_index < _images.size());

    if (_current_image_index > 0)
    {

        if (_current_image_index == _first_visible_image_index)
        {
            _first_visible_image_index -= _layout.width();
            UpdateImageWidgets();
        }

        --_current_image_index;
        HighlightCurrentWidget();
    }
}

void ImageLayoutWidget::Right()
{
    assert(_images.empty() || _current_image_index < _images.size());

    if (_current_image_index < _images.size() - 1)
    {
        if (_current_image_index == _first_visible_image_index + _layout.width() * _layout.height() - 1)
        {
            _first_visible_image_index += _layout.width();
            UpdateImageWidgets();
        }
        ++_current_image_index;
        HighlightCurrentWidget();
    }
}

void ImageLayoutWidget::LineDown()
{
    assert(_images.empty() || _current_image_index < _images.size());

    if(_layout.width() * _layout.height() == 1 &&
            _current_image_index < _images.size() - 1) // 1x1 matric
    {
        ++_first_visible_image_index;
    }
    else
    {   //After the current images on the last line of the page, the press on linedown is scroll one line.
        if(_current_image_index < _images.size() / _layout.width() * _layout.width() &&
                _current_image_index >= _first_visible_image_index + _layout.width() * (_layout.height() - 1))
        {
            //In case screen scrolls down even when the last line of images vector is filled.
            if(!(_images.size() % _layout.width() == 0 && _current_image_index > _images.size() - _layout.width() - 1) &&
                    _images.size() - _layout.width() != 0)
            {
                _first_visible_image_index += _layout.width();
            }
        }
    }
    _current_image_index = min(_current_image_index + _layout.width(), unsigned(_images.size() - 1));

    UpdateImageWidgets();
    HighlightCurrentWidget();
}

void ImageLayoutWidget::LineUp()
{
    assert(_images.empty() || _current_image_index < _images.size());

    if (_current_image_index < _first_visible_image_index + _layout.width())
    {
        if (_first_visible_image_index != 0)
        {
            _first_visible_image_index -= _layout.width();
            UpdateImageWidgets();
        }
    }

    _current_image_index = (int(_current_image_index) < _layout.width()) ?
                0 : _current_image_index - _layout.width();

    HighlightCurrentWidget();
}

void ImageLayoutWidget::Home()
{
    assert(_images.empty() || _current_image_index < _images.size());

    _current_image_index = 0;
    _first_visible_image_index = 0;

    HighlightCurrentWidget();

    UpdateImageWidgets();
}

void ImageLayoutWidget::End()
{
    assert(_images.empty() || _current_image_index < _images.size());

    _current_image_index = unsigned int(_images.size() - 1);
    _first_visible_image_index = unsigned int((_images.size() - 1) /
                                    (_layout.width()*_layout.height()) *
                                    (_layout.width() * _layout.height()));

    HighlightCurrentWidget();
    UpdateImageWidgets();
}

void ImageLayoutWidget::OnFocusChange(ImageWidget *sender)
{
    setFocus();

    for (int i = 0; i < _layout.width() * _layout.height(); ++i)
    {
        if (sender  == _image_widgets[i])
        {
            _current_image_index = unsigned int(i + _first_visible_image_index);
            break;
        }
    }
    HighlightCurrentWidget();

}

 std::vector<ImageWidget*>& ImageLayoutWidget::GetImageWidgets()
 {
     return _image_widgets;
 }
