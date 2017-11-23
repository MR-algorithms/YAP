#ifndef Segment3D_h__20100614
#define Segment3D_h__20100614

#include "geometry.h"
#include "Line3D.h"
namespace Geometry
{
	class CSegment3D
	{
	public:
        CSegment3D(const Point3D& end_point_1, const Point3D& end_point_2);
		CSegment3D();
        const CLine3D& GetLine() const;
        bool Contains(const Point3D& point) const;

        Point3D& FirstPoint();
        Point3D& SecondPoint();
	protected:
        CLine3D _line;

        Point3D _point_1;
        Point3D _point_2;
	};
}


#endif // Segment3D_h__20100614
