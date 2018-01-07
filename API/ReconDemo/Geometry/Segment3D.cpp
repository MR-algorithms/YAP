#include "Segment3D.h"
#include <float.h>
#include "algorithm.h"
using namespace Geometry;

CSegment3D::CSegment3D()
{
}

CSegment3D::CSegment3D(const Point3D& end_point_1,
                       const Point3D& end_point_2)
    : _line(end_point_1, Vector3D(end_point_1.x - end_point_2.x, end_point_1.y - end_point_2.y, end_point_1.z - end_point_2.z))
	, _point_1(end_point_1)
	, _point_2(end_point_2)
{
}

const CLine3D& CSegment3D::GetLine() const
{
	return _line;
}

bool CSegment3D::Contains(const Point3D& point) const
{
    return (fabs(Geometry::Distance(_point_1, _point_2) - Geometry::Distance(point, _point_1) - Geometry::Distance(point, _point_2)) <= FLT_EPSILON);
}

Point3D& CSegment3D::FirstPoint()
{
	return _point_1;
}

Point3D& CSegment3D::SecondPoint()
{
	return _point_2;
}
