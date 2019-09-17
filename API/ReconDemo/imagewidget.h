#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include "Interface/Interfaces.h"
#include "imagewindowmapper.h"
#include "utility"
#include "Geometry/Rect3D.h"

class ImageObject;

class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = 0);
    bool SetImage(Yap::IData* data, bool repaint = false);
    void Clear(bool repaint = false);

    void SetHighlight(bool highlight, bool repaint = false);
    bool IsHighlighted() const;

    void ZoomFit();
    void ZoomFill();
    void AddImageObject(std::shared_ptr<ImageObject> object, bool reset_objects);
    std::pair<bool, Geometry::CRect3D> GetImageRect();

    void SetVector(double x1, double y1, double z1, double x2, double y2, double z2);//临时函数

signals:
    void FocusSet(ImageWidget * sender);

public slots:
private:
    Yap::SmartPtr<Yap::IData> _data;
    Yap::SmartPtr<Yap::IVariableContainer> _properties;
    ImageWindowMapper _mapper;
    void DrawImageObjects(QPainter& painter, ImageWindowMapper& image_mapper);

protected:
    void paintEvent(QPaintEvent *event);
    enum ZoomMode {ZoomModeFit, ZoomModeFill, ZoomModeManual} _zoom_mode;
    void SetImageRect();
    unsigned short _window_level;
    unsigned short _window_width;
    double _image_size_ratio;

    // QWidget interface
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;


    QPoint _last_point;
    bool _highlight;
    Geometry::CRect3D _image_rect;
    std::vector<std::shared_ptr<ImageObject>> _image_objects;
};

#endif // IMAGEWIDGET_H
