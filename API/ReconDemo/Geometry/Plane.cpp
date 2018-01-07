#include <float.h>
#include "Plane.h"
#include <math.h>

using namespace Geometry;

CPlane::CPlane(void)
{
}

CPlane::CPlane(const Point3D& m0,
               const Vector3D& n) :
	_m0(m0), _n(n)
{
}

bool CPlane::Contains(const Point3D& point) const
{
    Vector3D v1 = _n;
    Vector3D v2(point.x - _m0.x, point.y - _m0.y, point.z - _m0.z);
    double dot = v1.cx * v2.cx + v1.cy * v2.cy + v1.cz * v2.cz;

    return fabs(dot) < FLT_EPSILON;
}

