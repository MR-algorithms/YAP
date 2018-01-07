#ifndef CRECT2D_H
#define CRECT2D_H

#include "Geometry/geometry.h"

namespace Geometry
{
	/**
		This class is the same in the meaning with the MFC CRect class, but all
		coordinates are stored in double, so it can be used to store a real world
		rectangle.
	*/
	class CRect2D  
	{
	public:
		CRect2D();
		CRect2D(double left, double top, double right, double bottom);
        CRect2D(const Point2D& point, const Vector2D& size);
		virtual ~CRect2D(){};

        bool Contains(const Point2D& point);
		void Normalize();
		void InflateRect(double delta_x, double delta_y);

		double _left;
		double _right;
		double _top;
		double _bottom;
	};

}	// End namespace Geometry.

#endif // !defined(AFX_GENERICRECT_H__C54747AE_2A76_4514_B853_ABE6B55065E1__INCLUDED_)
