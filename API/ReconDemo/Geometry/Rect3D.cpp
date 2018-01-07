#include "Rect3D.h"
#include "algorithm.h"
using namespace std;
using namespace Geometry;

CRect3D::CRect3D(void) 
	: _matrix_updated(false)
	, _x(1.0, 0, 0)
	, _y(0, 1.0, 0)
	, _center(0, 0, 0)
	, _cx(0)
	, _cy(0)
{
}

CRect3D::~CRect3D(void)
{
}


double CRect3D::GetCx() const
{
	return _cx;
}

void CRect3D::SetCx(double cx)
{
	_cx = cx;
}

double CRect3D::GetCy() const
{
	return _cy;
}

void CRect3D::SetCy(double cy)
{
	_cy = cy;
}

const Point3D& CRect3D::Center() const
{
	return _center;
}

void CRect3D::SetCenter(const Point3D& center)
{
	_center = center;
}

/// Sets the Direction of slice /phase/ frequency of the slice.
void  CRect3D::SetOrientation(const Vector3D& x,
                              const Vector3D& y)
{
	_x = x;
    Geometry::Normalize(_x);
	_y = y;
    Geometry::Normalize(_y);
}

const CMatrix44& CRect3D::SpaceToSlice()
{
	if (!_matrix_updated)
	{
		UpdateMatrix();
	}
	return _space_to_slice;
}

const CMatrix44& CRect3D::SliceToSpace()
{
	if (!_matrix_updated)
	{
		UpdateMatrix();
	}
	return _slice_to_space;
}

/// Rotates the slice around a given axis.
/*CRect3D&   CRect3D::Rotate(const Vector3D& axis,
						   double angle)
{
	CMatrix44 rotate = CMatrix44::Rotate(axis, CRadian(angle));

	_x = rotate * _x;
	_y = rotate * _y;

	_matrix_updated = false;

	return (*this);
}*/

/// Translates the slice in the 3d space.
/*CRect3D& CRect3D::MoveTo(const CPoint3D<double>& point)
{
	if (point != _center)
	{
		_center = point;
		_matrix_updated = false;
	}
	return *this;
}*/

/// Returns the frequency encoding direction of the slice.
Vector3D CRect3D::GetXOrientation() const
{
	return _x;
}

/// Returns the phase encoding direction of the slice.
Vector3D CRect3D::GetYOrientation() const
{
	return _y;
}

/// Returns the slice direction (norm of the slice plane) of the slice. 
Vector3D CRect3D::GetZOrientation() const
{  
    return Vector3D(_x.cy * _y.cz - _x.cz * _y.cy,
                    _x.cz * _y.cx - _x.cx * _y.cz,
                    _x.cx * _y.cy - _x.cy * _y.cx);
}



void CRect3D::UpdateMatrix()
{
	CMatrix44 rotate = CMatrix44::FromBasisVectors(_x, _y, GetZOrientation());

    _space_to_slice = rotate.GetTranspose() * CMatrix44::Translate(Vector3D(-_center.x, -_center.y, -_center.z));
    Vector3D center(_center.x, _center.y, _center.z);
    _slice_to_space = CMatrix44::Translate(center) * rotate;

	_matrix_updated = true;
}


std::vector<Point3D> CRect3D::GetVertices()
{
    Vector3D offset_x(_cx * _x.cx * 0.5, _cx * _x.cy * 0.5,  _cx * _x.cz * 0.5);
    Vector3D offset_y(_cy * _y.cx * 0.5, _cy * _y.cy * 0.5, _cy * _y.cz * 0.5);

    std::vector<Point3D> vertices;
    vertices.push_back(Point3D(_center.x + offset_x.cx + offset_y.cx,
                               _center.y + offset_x.cy + offset_y.cy,
                               _center.z + offset_x.cz + offset_y.cz));
    vertices.push_back(Point3D(_center.x - offset_x.cx + offset_y.cx,
                               _center.y - offset_x.cy + offset_y.cy,
                               _center.z - offset_x.cz + offset_y.cz));
    vertices.push_back(Point3D(_center.x - offset_x.cx - offset_y.cx,
                               _center.y - offset_x.cy - offset_y.cy,
                               _center.z - offset_x.cz - offset_y.cz));
    vertices.push_back(Point3D(_center.x + offset_x.cx - offset_y.cx,
                               _center.y + offset_x.cy - offset_y.cy,
                               _center.z + offset_x.cz - offset_y.cz));

	return vertices;
}

std::pair<bool, CSegment3D> CRect3D::GetIntersection(const CPlane& plane)
{
    std::vector<Point3D> vertices = GetVertices();
    std::vector<Point3D> points;

    std::pair<bool, Point3D> intersection = Geometry::GetIntersection(CSegment3D(vertices[0], vertices[1]), plane);
	if (intersection.first)
	{
		points.push_back(intersection.second);
	}

	intersection = Geometry::GetIntersection(CSegment3D(vertices[2], vertices[3]), plane);
	if(intersection.first)
	{
		points.push_back(intersection.second);
	}

	// if we found two points, jump over the following step. And the two
	// points are the edges of slice along Frequency encoding direction.
	if(points.size() != 2)
	{
		intersection = Geometry::GetIntersection(CSegment3D(vertices[3], vertices[0]), plane);
		if (intersection.first)
		{
			points.push_back(intersection.second);
		}

		if(points.size() != 2)
		{
			intersection = Geometry::GetIntersection(CSegment3D(vertices[1], vertices[2]), plane);
			if (intersection.first)
			{
				points.push_back(intersection.second);
			}
		}
	}

	// if there are two points, we put them into the m_EndPoints2 container.
	if(points.size() == 2)
	{
		return std::make_pair(true, CSegment3D(points[0], points[1]));
	}
	else
	{
		return std::make_pair(false, CSegment3D());
	}
}

CPlane CRect3D::GetPlane() const
{
	return CPlane(_center, GetZOrientation());
}

bool Geometry::operator==(const CRect3D& first, const CRect3D& second)
{
	return ((first._center == second._center) &&
		(first.GetZOrientation() == second.GetZOrientation()) &&
		(fabs(first._cx - second._cx) < FLT_EPSILON) &&
		(fabs(first._cy - second._cy) < FLT_EPSILON));	
}

bool Geometry::operator!=(const CRect3D& first, const CRect3D& second)
{
	return !(first == second);
}
