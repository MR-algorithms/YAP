#ifndef IMAGEWINDOWMAPPER_H
#define IMAGEWINDOWMAPPER_H
#include <QSize>
#include <QPoint>
#include "Geometry/geometry.h"
#include "Geometry/Matrix33.h"

class ImageWindowMapper
{
public:
    ImageWindowMapper();
    ~ImageWindowMapper();

    void SetRotationAngle(double angle);
    double GetRotationAngle() const;

    bool IsVerticalFlip() const;
    void SetVerticalFlip(bool flip);
    bool IsHorizontalFlip() const;
    void SetHorizontalFlip(bool flip);

    void SetZoom(double dZoom);
    double GetZoom() const;

    void SetPane(const Vector2D& pane);
    Vector2D GetPane() const;

    void HorizontalFlip();
    void VerticalFlip();
    void Rotate(double angle);
    void Zoom(double multiple);
    void Pane(const Vector2D& offset);

    void Reset();

    bool UpdateMapper();

    void SetFOV(double dFovX, double dFovY);
    void SetImageSize(QSize size);
    void SetWindowSize(QSize size);
    void SetWindowOffset(const QSize& offset);

    QSize GetWindowSize() const {
        return m_sizeWnd;
    }
    const QSize& GetWindowOffset() const{
        return _window_offset;
    }

    QPoint WindowToVoxel(const QPoint& pt_window) const;
    QPoint VoxelToWindow(const Point2D& voxel_point) const;

    QPoint PhysicalToVoxel(const Point2D& ptPhysical) const;
    Point2D PhysicalToDoubleVoxel(const Point2D& ptPhysical) const;
    Point2D VoxelToPhysical(const Point2D& voxel) const;


    QPoint PhysicalToWindow(const Point2D& ptPhysical) const;
    QSize PhysicalToWindow(const Vector2D& size_physical) const;

    Point2D PhysicalToWindow2D(const Point2D& ptPhysical) const;
    Vector2D PhysicalToWindow2D(const Vector2D& ptPhysical) const;

    Point2D WindowToPhysical(const QPoint& ptWindow) const;
    Vector2D WindowToPhysical(const QSize& size) const;

    bool operator == (const ImageWindowMapper& rhs);

protected:
    double  m_dAngle;       //!< Rotated angle in arc.
    bool    m_bVFlip;       //!< Vertical flip?
    bool    m_bHFlip;       //!< Horizontal flip?
    double  m_dZoom;        //!< Zoom ratio.
    Point2D m_sizePane;     //!< Pane offset, in original image, in voxel.
    double  m_dFovX;        //!< X dimension of FOV.
    double  m_dFovY;        //!< Y dimension of FOV.
    QSize   m_sizeImage;    //!< Size of the original image in voxel.
    QSize   m_sizeWnd;      //!< Size of the window in pixel.
    bool	m_bUpdated;		//!< Whether the constants has been recalculated.
    QSize	_window_offset;

    Geometry::CMatrix33 _window_to_voxel;
    Geometry::CMatrix33 _voxel_to_window;

    Geometry::CMatrix33 _window_to_physical;
    Geometry::CMatrix33 _physical_to_window;

    Geometry::CMatrix33 _voxel_to_physical;
    Geometry::CMatrix33 _physical_to_voxel;

    Geometry::CMatrix33 CalcVoxelToWindow() const;
    Geometry::CMatrix33 CalcPhysicalToVoxel() const;
};

#endif // IMAGEWINDOWMAPPER_H
