#ifndef Rect3D_h__20100614
#define Rect3D_h__20100614

#include "geometry.h"
#include "Segment3D.h"
#include "Plane.h"

#include <vector>
#include <utility>
#include "cmatrix44.h"

namespace Geometry
{
	class CRect3D
	{
	public:
		CRect3D(void);
		~CRect3D(void);
		double GetCy() const;
		void SetCy(double cy);

		double GetCx() const;
		void SetCx(double cx);

        const Point3D& Center() const;
        void SetCenter(const Point3D& center);

		/// Sets the Direction of slice the slice
        void  SetOrientation(const Vector3D& x, const Vector3D& y);
		/// Returns the frequency encoding direction of the slice.
        Vector3D GetXOrientation() const;
		/// Returns the phase encoding direction of the slice.
        Vector3D GetYOrientation() const;
		/// Returns the slice direction (norm of the slice plane) of the slice. 
        Vector3D GetZOrientation() const;

		CPlane GetPlane() const;

		/// Rotates the slice around a given axis.
        CRect3D& Rotate(const Vector3D& axis, double angle);
		/// Translates the slice in the 3d space.
        CRect3D& MoveTo(const Point3D& point);

        std::vector<Point3D> GetVertices();

        const CMatrix44& SpaceToSlice();
        const CMatrix44& SliceToSpace();

		std::pair<bool, CSegment3D> GetIntersection(const CPlane& plane);

		friend bool operator==(const CRect3D& first, const CRect3D& second);
		friend bool operator!=(const CRect3D& first, const CRect3D& second);

	protected:
        Point3D _center;
		double _cx;
		double _cy;
        Vector3D _x;
        Vector3D _y;

		void UpdateMatrix();

        CMatrix44   _space_to_slice;
        CMatrix44   _slice_to_space;

		bool _matrix_updated;
	};
};

#endif // Rect3D_h__20100614
