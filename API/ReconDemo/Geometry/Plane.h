#ifndef CPlane_H
#define CPlane_H

#include "geometry.h"

namespace Geometry
{
	class CPlane
	{
	public:
		CPlane(void);
        CPlane(const Point3D& m0, const Vector3D& n);

        bool Contains(const Point3D& point) const;
        Point3D _m0;
        Vector3D _n;
	};
};
#endif

