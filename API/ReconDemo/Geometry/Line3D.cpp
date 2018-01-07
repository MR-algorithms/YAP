#include "Line3D.h"
#include "plane.h"
#include <float.h>
#include "algorithm.h"
#include <cassert>

using namespace Geometry;



CLine3D::CLine3D(void)
{
}

CLine3D::CLine3D(const Point3D& m0,
                 const Vector3D& s)
{
	_m0 = m0;
	_s  = s;
}

CLine3D::~CLine3D(void)
{
}


Point3D CLine3D::GetFootOfPerpendicular(Point3D point) const
{
	CPlane plane(point, _s);

    return Geometry::GetIntersection(*this, plane).second;
}

bool CLine3D::Contains(const Point3D& point) const
{
    Vector3D v1 = _s;
    Vector3D v2(point.x - _m0.x, point.y - _m0.y, point.z - _m0.z);
    Normalize(v1);
    Normalize(v2);
    Vector3D cross_product(v1.cy * v2.cz - v1.cz * v2.cy,
                           v1.cz * v2.cx - v1.cx * v2.cz,
                           v1.cx * v2.cy - v1.cy * v2.cx);

    return (fabs(cross_product.cx) <= FLT_EPSILON && fabs(cross_product.cy) <= FLT_EPSILON && fabs(cross_product.cz) <= FLT_EPSILON);
}



