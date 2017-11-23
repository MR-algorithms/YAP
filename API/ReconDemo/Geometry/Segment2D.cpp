#include "Segment2d.h"

#include <cmath>
#include <float.h>

#include "rect2d.h"
#include "algorithm.h"

using namespace Geometry;

CSegment2D::~CSegment2D()
{
}

CSegment2D::CSegment2D()
	: _end_point_1(0.0, 0.0)
	, _end_point_2(0.0, 0.0)
{
}

CSegment2D::CSegment2D(const Point2D& point1,
                       const Point2D& point2)
	: _end_point_1(point1)
	, _end_point_2(point2)
{
}

const Point2D& CSegment2D::GetFirstPoint() const
{
	return _end_point_1;
}

const Point2D& CSegment2D::GetSecondPoint() const
{
	return _end_point_2;
}

bool CSegment2D::Contains(const Point2D& point) const
{
    return (fabs(Geometry::Distance(point, _end_point_1) + Geometry::Distance(point, _end_point_2) - Length()) < FLT_EPSILON);
}

bool CSegment2D::Contains(const Point2D& point,
						  double tolerance) const
{
	if (tolerance < 1)
	{
		tolerance = 1;
	}

	CRect2D rc(_end_point_1.x, _end_point_1.y, _end_point_2.x, _end_point_2.y);
	rc.Normalize();
	rc.InflateRect(tolerance, tolerance);

    return (rc.Contains(point) && Geometry::DistanceToLine (point, _end_point_1, _end_point_2) <= tolerance);
}

double CSegment2D::Length() const
{
    return Geometry::Distance(_end_point_1, _end_point_2);
}

Point2D Geometry::CSegment2D::Center() const
{
    return Point2D((_end_point_1.x + _end_point_2.x) / 2.0, (_end_point_1.y + _end_point_2.y) / 2.0);
}
