
#include "Rect2D.h"
#include <utility>
#include <assert.h>

using namespace Geometry;

/**
	Default constructor. All members initialized to zero.
*/
 CRect2D::CRect2D()
{
	_left = _right = _top = _bottom = 0.0;
}

/**
	Constructs an object from the four side of the rectangle.
*/
 CRect2D::CRect2D(double left, 
                  double top,
                  double right,
                  double bottom)
{
	_left = left;
	_right = right;
	_top = top;
	_bottom = bottom;
}

/**
	Normalize the rectangle, that is, to make sure the left side of rectangle
	is less than the right and the top is less than the bottom.
*/
 void CRect2D::Normalize()
{
	if (_left > _right)
	{
		std::swap(_left, _right);
	}
	if (_top > _bottom)
	{
		std::swap(_top, _bottom);
	}
}

/**
	Check to see if a given point is in the rectangle.
	@param pt Point to test.
	\pre The rectangle must be normalized before calling this function.
	You can call NormallizeRect() to normalize the rectangle.
*/
 bool CRect2D::Contains(const Point2D& pt)
{
	// the rectangle must be normalized before calling this function.
	assert((_left <= _right) && (_top <= _bottom));

	return (pt.x >= _left && pt.x < _right && pt.y >= _top && pt.y < _bottom);
}

/**
	Inflate the rectangle by the given value.
	@param dDeltaX Amount to inflate on left and right side.
	@param dDeltaY Amount to inflate on top and bottom side.
	\pre The rectangle must be normalized before calling this function.
*/
 void CRect2D::InflateRect(double delta_x, double delta_y)
{
	// the rectangle must be normalized before calling this function.
	assert((_left <= _right) && (_top <= _bottom));

	_left -= delta_x;
	_right += delta_x;
	_top -= delta_y;
	_bottom += delta_y;
}

/// Constructor.
/**
	This function constructs a CGenericRect from its top left corner and size.
	@param point Coordinate ot the top left corner of the rectangle.
	@param size Size of the rectangle.
*/
 CRect2D::CRect2D(const Point2D& point,
                  const Vector2D& size)
{
	_left = point.x;
    _right = point.x + size.cx;
	_top = point.y;
    _bottom = point.y + size.cy;
}
