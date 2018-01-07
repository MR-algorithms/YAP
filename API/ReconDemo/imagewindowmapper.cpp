#include "imagewindowmapper.h"
//#include <boost/math/special_functions/round.hpp>
#include "rectcalc.h"
#include "Geometry/Matrix33.h"
#include "Geometry/Angle.h"
#include "assert.h"
#define FLOAT_ZERO 1.0e-30
//using boost::math::lround;

/*int round(double val)
{
    return (val > 0.0) ? floor(val + 0.5) : ceil(val - 0.5);
}*/
using namespace Geometry;
ImageWindowMapper::ImageWindowMapper()
:m_sizePane (0, 0)
, m_sizeImage(0, 0)
, m_sizeWnd(0, 0)
, _window_offset(0, 0)
{
    m_dAngle = 0.0;
    m_bVFlip = false;
    m_bHFlip = false;
    m_dZoom = 1.0;
    m_dFovX = 0.0;
    m_dFovY = 0.0;
    m_bUpdated = false;
}

ImageWindowMapper::~ImageWindowMapper()
{
}

bool ImageWindowMapper::UpdateMapper()
{
    if (m_bUpdated)
    {
        return true;
    }
    if (m_sizeImage.width() <= 0 || m_sizeImage.height() <= 0 ||
        m_dFovX < FLOAT_ZERO || m_dFovY < FLOAT_ZERO ||
        m_sizeWnd.width() <= 0 || m_sizeWnd.height() <= 0)
    {
        CMatrix33 identity;
        _voxel_to_window = identity;
        _window_to_voxel = identity;
        _physical_to_window = identity;
        _window_to_physical = identity;
        _voxel_to_physical = identity;
        _physical_to_voxel = identity;

        m_bUpdated = true;
        return false;
    }
    _voxel_to_window = CalcVoxelToWindow();
    _voxel_to_window.GetInverse(_window_to_voxel);

    CMatrix33 physical_to_voxel = CalcPhysicalToVoxel();

    _physical_to_window = _voxel_to_window * physical_to_voxel;
    _physical_to_window.GetInverse(_window_to_physical);

    _voxel_to_physical = _window_to_physical * _voxel_to_window;
    _physical_to_voxel = _window_to_voxel * _physical_to_window;

    m_bUpdated = true;
    return true;
}

QPoint ImageWindowMapper::WindowToVoxel(const QPoint& ptWindow) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();

    Point2D p = _window_to_voxel * Point2D(ptWindow.x(), ptWindow.y());

  //  return QPoint(lround(p.x), lround(p.y));
    return QPoint(p.x, p.y);
}

/*int int_round(double v)
{
    try
    {
        return boost::math::iround(v);
    }
    catch (std::exception& e)
    {
        LOG_ERROR(CString(e.what()));
        if (v < 0)
        {
            return INT_MIN;
        }
        else
        {
            return INT_MAX;
        }
    }
}*/

QPoint ImageWindowMapper::PhysicalToWindow(const Point2D& ptPhysical) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();
    Point2D p = _physical_to_window * ptPhysical;

    //return QPoint(int_round(p.x) - _window_offset.cx, int_round(p.y) - _window_offset.cy);
    return QPoint(p.x - _window_offset.width(), p.y - _window_offset.height());
}

QSize ImageWindowMapper::PhysicalToWindow(const Vector2D& size_physical) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();

    auto size = _physical_to_window * size_physical;

    //return QSize(int_round(size.x), int_round(size.y));
    return QSize(size.cx, size.cy);
}

Point2D ImageWindowMapper::PhysicalToWindow2D(const Point2D& ptPhysical) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();
    Point2D pt = _physical_to_window * ptPhysical;
    pt.x = pt.x - _window_offset.width();
    pt.y = pt.y - _window_offset.height();
    return pt;
}

Vector2D ImageWindowMapper::PhysicalToWindow2D(const Vector2D& vPhysical) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();
    Vector2D size = _physical_to_window * vPhysical;
    return size;
}

Point2D ImageWindowMapper::WindowToPhysical(const QPoint& ptWindow) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();

    Point2D pt_window(ptWindow.x() + _window_offset.width(),
                      ptWindow.y() + _window_offset.height());
    Point2D pt = _window_to_physical * pt_window;
    return pt;
}

Vector2D ImageWindowMapper::WindowToPhysical(const QSize& szWindow) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();

    Vector2D size_window(szWindow.width(), szWindow.height());
    Vector2D size2d = _window_to_physical * size_window;
    return size2d;
}

QPoint ImageWindowMapper::PhysicalToVoxel(const Point2D& ptPhysical) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();
    assert(m_dFovX > FLOAT_ZERO && m_dFovY > FLOAT_ZERO);
    Point2D pt = CalcPhysicalToVoxel() * ptPhysical;
  //  QPoint ptVoxel(boost::math::lround(pt.x), boost::math::lround(pt.y));
    QPoint ptVoxel(pt.x, pt.y);
    return ptVoxel;
}

Point2D ImageWindowMapper::PhysicalToDoubleVoxel(const Point2D& ptPhysical) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();
    assert(m_dFovX > FLOAT_ZERO && m_dFovY > FLOAT_ZERO);
    Point2D pt = CalcPhysicalToVoxel() * ptPhysical;
    return pt;
}


Point2D ImageWindowMapper::VoxelToPhysical(const Point2D& voxel) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();

    return _voxel_to_physical * voxel;
}

QPoint ImageWindowMapper::VoxelToWindow(const Point2D& voxel_point) const
{
    const_cast<ImageWindowMapper*>(this)->UpdateMapper();
    auto window_point = _voxel_to_window * voxel_point;

   // return QPoint(int_round(window_point.x), int_round(window_point.y));
    return QPoint(window_point.x, window_point.y);
}

void ImageWindowMapper::SetFOV(double dFovX, double dFovY)
{
    if (abs(m_dFovX - dFovX) < 0.000001 && abs(m_dFovY - dFovY) < 0.000001)
    {
        return;
    }
    m_dFovX = dFovX;
    m_dFovY = dFovY;
    m_bUpdated = false;
}

void ImageWindowMapper::SetImageSize(QSize size)
{
    if (m_sizeImage == size)
    {
        return;
    }

    m_sizeImage = size;
    m_bUpdated = false;
}

void ImageWindowMapper::SetWindowSize(QSize size)
{
    if (m_sizeWnd == size)
    {
        return;
    }

    m_sizeWnd = size;
    m_bUpdated = false;
}

void ImageWindowMapper::SetWindowOffset(const QSize& offset)
{
    if (_window_offset == offset)
    {
        return;
    }

    _window_offset = offset;
    m_bUpdated = false;
}

CMatrix33 ImageWindowMapper::CalcVoxelToWindow() const
{
    CMatrix33 rotation;
    rotation.MakeRotate(CRadian(-m_dAngle));

    CMatrix33 flip;
    if (m_bHFlip)
    {
        flip = flip * CMatrix33::FromBasisVectors(Vector2D(-1.0, 0.0), Vector2D(0.0, 1.0));
    }
    if (m_bVFlip)
    {
        flip = flip * CMatrix33::FromBasisVectors(Vector2D(1.0, 0.0), Vector2D(0.0, -1.0));
    }

    CMatrix33 pane;
    pane.MakeTranslate(Vector2D(m_sizePane.x, m_sizePane.y));

    QSize image_rect_size = RectCalc::GetFitSize(m_sizeImage, m_sizeWnd);
    double fit_ratio = image_rect_size.width()/ static_cast<double>(m_sizeImage.width());
    CMatrix33 zoom;
    zoom.MakeScale(Vector2D(m_dZoom * fit_ratio, m_dZoom * fit_ratio));

    CMatrix33 image_center_to_first;
    image_center_to_first.MakeTranslate(Vector2D(-(m_sizeImage.width() - 1) / 2.0, -(m_sizeImage.height() - 1) / 2.0));

    CMatrix33 viweport_first_to_center;
    viweport_first_to_center.MakeTranslate(Vector2D((m_sizeWnd.width() - 1) / 2.0, (m_sizeWnd.height() - 1) / 2.0));

    return viweport_first_to_center * zoom * pane * flip * rotation * image_center_to_first;
}

CMatrix33 ImageWindowMapper::CalcPhysicalToVoxel() const
{
    CMatrix33 scale;
    scale.MakeScale(Vector2D(m_sizeImage.width() / m_dFovX, m_sizeImage.height() / m_dFovY));

    CMatrix33 translate;
    translate.MakeTranslate(Vector2D(m_sizeImage.width() / 2.0, m_sizeImage.height() / 2.0));

    return translate * scale;
}

void ImageWindowMapper::SetRotationAngle( double angle )
{
    if (m_dAngle == angle)
    {
        return;
    }

    m_dAngle = angle;
    m_bUpdated = false;
}

double ImageWindowMapper::GetRotationAngle() const
{
    return m_dAngle;
}

bool ImageWindowMapper::IsVerticalFlip() const
{
    return m_bVFlip;
}

void ImageWindowMapper::SetVerticalFlip( bool flip )
{
    if (m_bVFlip == flip)
    {
        return;
    }

    m_bVFlip = flip;
    m_bUpdated = false;
}

bool ImageWindowMapper::IsHorizontalFlip() const
{
    return m_bHFlip;
}

void ImageWindowMapper::SetHorizontalFlip( bool flip )
{
    if (m_bHFlip == flip)
    {
        return;
    }

    m_bHFlip = flip;
    m_bUpdated = false;
}

void ImageWindowMapper::SetZoom( double dZoom )
{
    if (m_dZoom == dZoom)
    {
        return;
    }

    m_dZoom = dZoom;
    m_bUpdated = false;
}

double ImageWindowMapper::GetZoom() const
{
    return m_dZoom;
}

void ImageWindowMapper::SetPane(const Vector2D& pane)
{
    if (m_sizePane.x == pane.cx && m_sizePane.y == pane.cy)
    {
        return;
    }

    m_sizePane = Point2D(pane.cx, pane.cy);
    m_bUpdated = false;
}

Vector2D ImageWindowMapper::GetPane() const
{
    return Vector2D(m_sizePane.x, m_sizePane.y);
}

void ImageWindowMapper::HorizontalFlip()
{
    m_bHFlip = !m_bHFlip;
    m_sizePane.x *=  -1;
    m_bUpdated = false;
}

void ImageWindowMapper::VerticalFlip()
{
    m_bVFlip = ! m_bVFlip;
    m_sizePane.y *= -1;
    m_bUpdated = false;
}

void ImageWindowMapper::Rotate( double angle )
{
    Vector2D old_pane(m_sizePane.x, m_sizePane.y);
    m_sizePane = Point2D(old_pane.cx * cos(angle) + old_pane.cy * sin(angle),
        -old_pane.cx * sin(angle) + old_pane.cy * cos(angle));

    angle = (m_bHFlip == m_bVFlip) ? angle : - angle;
    m_dAngle += angle;

    m_bUpdated = false;
}

void ImageWindowMapper::Zoom( double multiple )
{
    SetZoom(m_dZoom * multiple);
}

void ImageWindowMapper::Pane( const Vector2D& offset )
{
    if (m_sizeImage.width() == 0 || m_sizeImage.height() == 0)
    {
        return ;
    }

    QSize image_rect_size = RectCalc::GetFitSize(m_sizeImage, m_sizeWnd);
    double effective_zoom = m_sizeImage.width() / (m_dZoom * image_rect_size.width());

    m_sizePane = Point2D(m_sizePane.x + offset.cx * effective_zoom, m_sizePane.y + offset.cy * effective_zoom);
    Point2D max_pane ((m_sizeImage.width() * 4.0) / 5, (m_sizeImage.height() * 4.0) / 5);
    if (m_sizePane.x > max_pane.x)
    {
        m_sizePane.x = max_pane.x;
    }
    else if (m_sizePane.x < -max_pane.x)
    {
        m_sizePane.x = -max_pane.x;
    }
    if (m_sizePane.y > max_pane.y)
    {
        m_sizePane.y = max_pane.y;
    }
    else if (m_sizePane.y < -max_pane.y)
    {
        m_sizePane.y = -max_pane.y;
    }
    m_bUpdated = false;
}

bool ImageWindowMapper::operator==( const ImageWindowMapper& rhs )
{
    return ((fabs(m_dAngle - rhs.m_dAngle) <= 1.0e-5) &&
        m_bHFlip == rhs.m_bHFlip &&
        m_bVFlip == rhs.m_bVFlip &&
        (fabs(m_dZoom - rhs.m_dZoom) <= 1.0e-5) &&
        m_sizePane == rhs.m_sizePane);
}

void ImageWindowMapper::Reset()
{
    m_dZoom = 1.0;
    m_dAngle = 0.0;
    m_sizePane = Point2D(0.0, 0.0);
    m_bHFlip = false;
    m_bVFlip = false;

    m_bUpdated = false;
}
