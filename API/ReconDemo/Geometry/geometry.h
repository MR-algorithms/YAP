#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <math.h>
#include <float.h>

struct Point2D
{
    double x;
    double y;
    Point2D(): x(0), y(0){}
    Point2D(double x_, double y_): x(x_), y(y_){}
    bool operator == (const Point2D& rhs) const {
        return (x == rhs.x && y == rhs.y);
    }
};

struct Point3D
{
    double x;
    double y;
    double z;
    Point3D(): x(0), y(0), z(0){}
    Point3D(double x_, double y_, double z_): x(x_), y(y_), z(z_){}
    bool operator == (const Point3D& rhs) const {
        return (x == rhs.x && y == rhs.y && z == rhs.z);
    }
};

struct Vector2D
{
    double cx;
    double cy;
    Vector2D(): cx(0), cy(0){}
    Vector2D(double cx_, double cy_): cx(cx_), cy(cy_){}
    bool operator == (const Vector2D& rhs) const {
        return (cx == rhs.cx && cy == rhs.cy);
    }
};

struct Vector3D
{
    double cx;
    double cy;
    double cz;
    Vector3D(): cx(0), cy(0), cz(0){}
    Vector3D(double cx_, double cy_, double cz_): cx(cx_), cy(cy_), cz(cz_){}
    bool operator == (const Vector3D& rhs) const{
        return (cx == rhs.cx && cy == rhs.cy && cz == rhs.cz);
    }
};


#endif // GEOMETRY_H
