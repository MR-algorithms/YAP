#ifndef CLINE3D_H
#define CLINE3D_H

#include "geometry.h"


enum EAXIS{
	X,
	Y,
	Z,
};

namespace Geometry
{
	class CPlane;

	class CLine3D
	{
	public:
		CLine3D(void);
        CLine3D(const Point3D& m0, const Vector3D& s);
		~CLine3D(void);

        bool Contains(const Point3D& point) const;
        Point3D GetFootOfPerpendicular(Point3D point) const;

	public:
        Vector3D _s;
        Point3D _m0;
	};
};
#endif
