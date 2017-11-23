#include "algorithm.h"
#include "Line3D.h"
#include "Plane.h"
#include "Segment3D.h"
#include <assert.h>

using namespace Geometry;
std::pair<bool, Point3D> Geometry::GetIntersection(const CLine3D& line,
                                         const CPlane& plane)
{
    Point3D line_direction(line._s.cx, line._s.cy, line._s.cz);
    Point3D plane_normal_Direction(plane._n.cx, plane._n.cy, plane._n.cz);
    Normalize(line_direction);
    Normalize(plane_normal_Direction);
    double dRate = InnerProduct(plane_normal_Direction, line_direction);

    // 法线跟直线垂直（内积为0）时，直线跟平面没有交点。
    if (fabs(dRate) <= FLT_EPSILON)
    {
        return std::make_pair(false, Point3D());
    }

    double plane_m0 = InnerProduct(plane_normal_Direction, plane._m0); // 交点的参数
    double line_m0 = InnerProduct(plane_normal_Direction, line._m0);

    // line.m0为参考点,参数值t = 0;
    // 求出平面内投影点的参数t

    double t  = (plane_m0 - line_m0) / dRate;

    Point3D point = Point3D(line._m0.x + t * line_direction.x,
                            line._m0.y + t * line_direction.y,
                            line._m0.z + t * line_direction.z);

    // 检查交点是否在两个平面上。
    assert(line.Contains(point));
    assert(plane.Contains(point));

    return std::make_pair(true, point);
}

std::pair<bool, Point3D> Geometry::GetIntersection(const CSegment3D& segment,
                                                   const CPlane& plane)
{
    std::pair<bool, Point3D> intersection = GetIntersection(segment.GetLine(), plane);
    if (intersection.first && segment.Contains(intersection.second))
    {
        return intersection;
    }
    else
    {
        return std::make_pair(false, Point3D());
    }
}

double Geometry::Distance(const Point2D& pt1, const Point2D& pt2)
{
    double dx = pt1.x - pt2.x;
    double dy = pt1.y - pt2.y;
    return sqrt(dx * dx + dy * dy);
}

double Geometry::Distance(const Point3D &pt1, const Point3D &pt2)
{
    double dx = pt1.x - pt2.x;
    double dy = pt1.y - pt2.y;
    double dz = pt1.z - pt2.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

double Geometry::InnerProduct(const Point3D& pt1, const Point3D& pt2)
{
    return pt1.x * pt2.x + pt1.y * pt2.y + pt1.z * pt2.z;
}

double Geometry::DistanceToLine(const Point2D& pt, const Point2D& pt1, const Point2D& pt2)
{
    if (pt1.x == pt2.x && pt1.y == pt2.y)
        return Distance(pt, pt1);
    else
    {
        double a;
        double b;
        a = fabs(-pt2.y * pt1.x + pt2.x * pt1.y +
            pt2.y * pt.x - pt.x * pt1.y - pt2.x * pt.y + pt.y * pt1.x);
        b = Distance(pt1, pt2);
        return (a / b);
    }
}

bool Geometry::Normalize(Vector3D& v)
{
    double modular = sqrt(v.cx * v.cx + v.cy * v.cy + v.cz * v.cz);
    if(fabs(modular) <= FLT_EPSILON)
    {
        v.cx = v.cy = v.cz = 0.0;
        return false;
    }
    else
    {
        v.cx /= modular;
        v.cy /= modular;
        v.cz /= modular;
        return true;
    }
}

bool Geometry::Normalize(Point3D& pt)
{
    double modular = sqrt(pt.x * pt.x + pt.y * pt.y + pt.z * pt.z);
    if(fabs(modular) <= FLT_EPSILON)
    {
        pt.x = pt.y = pt.z = 0.0;
        return false;
    }
    else
    {
        pt.x /= modular;
        pt.y /= modular;
        pt.z /= modular;
        return true;
    }
}
