#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

class QPainter;
class ImageWindowMapper;

class ImageObject
{
public:
    ImageObject();
    virtual void Draw(QPainter& painter, ImageWindowMapper& mapper) = 0;
};

#endif // IMAGEOBJECT_H
