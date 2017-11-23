#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "geometry.h"
#include <utility>

namespace Geometry
{
    class CLine3D;
    class CPlane;
    class CSegment3D;
    std::pair<bool, Point3D> GetIntersection(const CLine3D& line,
                                             const CPlane& plane);
    std::pair<bool, Point3D> GetIntersection(const CSegment3D& segment,
                                             const CPlane& plane);
    bool Normalize(Point3D& pt);
    bool Normalize(Vector3D& v);
    double DistanceToLine(const Point2D& pt, const Point2D& pt1, const Point2D& pt2);
    double Distance(const Point3D &pt1, const Point3D &pt2);
    double Distance(const Point2D& pt1, const Point2D& pt2);
    double InnerProduct(const Point3D& pt1, const Point3D& pt2);
}

#endif // ALGORITHM_H
