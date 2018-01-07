#ifndef IMAGELAYOUTWIDGET_H
#define IMAGELAYOUTWIDGET_H

#include <QWidget>
#include "Interface/Interfaces.h"

namespace Yap{
    struct IData;
}

class ImageWidget;

class ImageLayoutWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageLayoutWidget(QWidget *parent = 0);

    bool AddImage(Yap::IData* data, Yap::IVariableContainer *properties = nullptr);
    const std::vector<Yap::SmartPtr<Yap::IData>> GetImages() const;
    void ClearImages();
    void ClearSingleImage();
    void BackupImage();

    bool SetLayout(QSize layout);
    QSize GetLayout() const;

    void PageDown();
    void PageUp();
    void Left();
    void Right();
    void LineDown();
    void LineUp();
    void Home();
    void End();

    void UpdateWidgetImage();

signals:
public slots:
    void OnFocusChange(ImageWidget* sender);

    void ZoomFit();
    void ZoomFill();
    ImageWidget * GetImageWidget(unsigned int image_index);
    std::vector<ImageWidget*>& GetImageWidgets();
private:
    std::vector<ImageWidget*> _image_widgets;
    std::vector<Yap::SmartPtr<Yap::IData>> _images;
    std::vector<Yap::SmartPtr<Yap::IData>> _images_erased;

    QSize _layout;
    const QSize _max_layout;
    unsigned int _first_visible_image_index;
    unsigned int _current_image_index;

    // QWidget interface
    void UpdateAll();
    unsigned int MakeImageVisible(unsigned int image_index) const;
    void HighlightCurrentWidget();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // IMAGELAYOUTWIDGET_H
