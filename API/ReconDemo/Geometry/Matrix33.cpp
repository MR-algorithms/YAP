#include "Matrix33.h"
#include "Angle.h"
#include <math.h>

namespace Geometry
{
	CMatrix33::CMatrix33(void)
	{
		Unit();
	}

	CMatrix33::CMatrix33(const CMatrix33& rhs)
	{
		for (int i = 0; i < 3 * 3; i++)
			_elements[i] = rhs._elements[i];
	}

	CMatrix33::~CMatrix33(void)
	{
	}

	void CMatrix33::Unit()
	{
		for(int i = 0; i < 3 * 3; i++)
		{
			_elements[i] = 0.0;
		}
		_elements[0*3 + 0] = 1.0;
		_elements[1*3 + 1] = 1.0;
		_elements[2*3 + 2] = 1.0;
	}

	Geometry::CMatrix33 CMatrix33::GetTranspose() const
	{
		CMatrix33 w;
		for(int index = 0; index < 3 * 3; index++)
			w._elements[index] = _elements[index];

		double   temp;
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < i; j++)
			{
				temp = w._elements[i*3 + j];
				w._elements[i*3 + j] = w._elements[j*3 + i];
				w._elements[j*3 + i] = temp;
			}
		}
		return w;
	}

	bool CMatrix33::GetInverse(CMatrix33& inverse) const
	{
		// Invert a 3x3 using cofactors.  This is about 8 times faster than
		// the Numerical Recipes code which uses Gaussian elimination.
		inverse._m[0][0] = _m[1][1]*_m[2][2] -
			_m[1][2]*_m[2][1];
		inverse._m[0][1] = _m[0][2]*_m[2][1] -
			_m[0][1]*_m[2][2];
		inverse._m[0][2] = _m[0][1]*_m[1][2] -
			_m[0][2]*_m[1][1];
		inverse._m[1][0] = _m[1][2]*_m[2][0] -
			_m[1][0]*_m[2][2];
		inverse._m[1][1] = _m[0][0]*_m[2][2] -
			_m[0][2]*_m[2][0];
		inverse._m[1][2] = _m[0][2]*_m[1][0] -
			_m[0][0]*_m[1][2];
		inverse._m[2][0] = _m[1][0]*_m[2][1] -
			_m[1][1]*_m[2][0];
		inverse._m[2][1] = _m[0][1]*_m[2][0] -
			_m[0][0]*_m[2][1];
		inverse._m[2][2] = _m[0][0]*_m[1][1] -
			_m[0][1]*_m[1][0];

		double fDet =
			_m[0][0]*inverse._m[0][0] +
			_m[0][1]*inverse._m[1][0]+
			_m[0][2]*inverse._m[2][0];

        if ( fabs(fDet) <= FLT_EPSILON )
			return false;

		double fInvDet = 1.0 / fDet;
		for (size_t iRow = 0; iRow < 3; iRow++)
		{
			for (size_t iCol = 0; iCol < 3; iCol++)
				inverse._m[iRow][iCol] *= fInvDet;
		}
		return true;
	}

	CMatrix33& CMatrix33::operator=( const CMatrix33& matrix )
	{
		if(this != &matrix)
		{
			for(int i = 0; i < 3 * 3; i++)
				_elements[i] = matrix._elements[i];
		}
		return *this;
	}

	CMatrix33& CMatrix33::operator*=( const CMatrix33& matrix )
	{
		double temp[9];
		for(int index = 0; index < 9; index++)
			temp[index] = 0;

		for(int i = 0; i < 3; i++)//ÐÐ
		{
			for(int j = 0; j < 3; j++)//ÁÐ
			{
				temp[i *3 +j] = 
					_elements[i*3    ] * matrix._elements[	    j] +
					_elements[i*3 + 1] * matrix._elements[1*3 + j] +
					_elements[i*3 + 2] * matrix._elements[2*3 + j];
			}
		}
		for(unsigned int index = 0; index < 3 * 3; index++)
			_elements[index] = temp[index];

		return *this;
	}

	CMatrix33 CMatrix33::Rotate(const CRadian& r)
	{
		CMatrix33 matrix;
		double cosine = cos(r.ValueRadians());
		double sine = sin(r.ValueRadians());
		matrix._00 = cosine;
		matrix._01 = -sine;
		matrix._10 = sine;
		matrix._11 = cosine;

		return matrix;
	}

	CMatrix33 CMatrix33::Translate(double x, double y)
	{
		CMatrix33 matrix;

		matrix._02 = x;
		matrix._12 = y;

		return matrix;
	}

	CMatrix33 CMatrix33::Scale(double fx, double fy)
	{
		CMatrix33 matrix;

		matrix._00 = fx;
		matrix._11 = fy;

		return matrix;
	}

	CMatrix33 CMatrix33::HFlip()
	{
		CMatrix33 matrix;
		matrix._00 = -1.0;

		return matrix;
	}

	CMatrix33 CMatrix33::VFlip()
	{
		CMatrix33 matrix;
		matrix._11 = -1.0;

		return matrix;
	}

	void CMatrix33::MakeRotate( const CRadian& r )
	{
		Unit();
		double cosine = cos(r.ValueRadians());
		double sine = sin(r.ValueRadians());
		_00 = cosine;
		_01 = -sine;
		_10 = sine;
		_11 = cosine;
	}

    void CMatrix33::MakeTranslate( const Vector2D& point )
	{
		Unit();
        _02 = point.cx;
        _12 = point.cy;
	}

    void CMatrix33::MakeScale( const Vector2D& scale )
	{
		Unit();
        _00 = scale.cx;
        _11 = scale.cy;
	}

    Geometry::CMatrix33 CMatrix33::FromTransform( const Vector2D& translate,
		const CRadian& r, 
        const Vector2D& scale )
	{
		CMatrix33 scale_matrix;
		scale_matrix.MakeScale(scale);

		CMatrix33 rotation;
		rotation.MakeRotate(r);

		CMatrix33 translate_matrix;
		translate_matrix.MakeTranslate(translate);

		return translate_matrix * rotation * scale_matrix;
	}

    Geometry::CMatrix33 CMatrix33::FromBasisVectors( const Vector2D& orient0, const Vector2D& orient1 )
	{
		CMatrix33 matrix;
        matrix._00 = orient0.cx, matrix._01 = orient1.cx;
        matrix._10 = orient0.cy, matrix._11 = orient1.cy;
		return matrix;
	}

    Vector2D operator*( const CMatrix33& matrix, const Vector2D& vec )
	{
        Vector2D w;
        w.cx = vec.cx * matrix._elements[0] + vec.cy * matrix._elements[1];
        w.cy = vec.cx * matrix._elements[3] + vec.cy * matrix._elements[4];
		return w;
	}

    Point2D operator*( const CMatrix33& matrix, const Point2D& point )
	{
        Point2D w;
		w.x = point.x * matrix._elements[0] + point.y * matrix._elements[1] + matrix._elements[2];
		w.y = point.x * matrix._elements[3] + point.y * matrix._elements[4] + matrix._elements[5];

		return w;
	}

	CMatrix33 operator*( const CMatrix33& u, const CMatrix33& v )
	{
		CMatrix33 w;
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				w._elements[i * 3 +j] = 
					u._elements[i * 3 + 0] * v._elements[0 * 3 + j] +
					u._elements[i * 3 + 1] * v._elements[1 * 3 + j] +
					u._elements[i * 3 + 2] * v._elements[2 * 3 + j];
			}
		}
		return w;
	}

}

