#ifndef Segment_h__20100628
#define Segment_h__20100628

#include <utility>
#include "geometry.h"
namespace Geometry
{
	class CSegment2D
	{
	public:
		CSegment2D();
        CSegment2D(const Point2D& point1, const Point2D& point2);

		virtual ~CSegment2D();

        const Point2D& GetFirstPoint() const;
        const Point2D& GetSecondPoint() const;
        Point2D Center() const;
        bool Contains(const Point2D& point) const;
        bool Contains(const Point2D& point, double tolerance) const;
		double Length() const;

	protected:
        Point2D _end_point_1;
        Point2D _end_point_2;
	};
}	// namespace Geometry

#endif // Segment_h__20100628
