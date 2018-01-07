// Copy From OgreMath.h and modify it

//OGRE (www.ogre3d.org) is made available under the MIT License.
//
//	Copyright (c) 2000-2009 Torus Knot Software Ltd
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.

#ifndef Angle_Degree_Radian_h__
#define Angle_Degree_Radian_h__


#define RadianPI (3.1415926535898)

namespace Geometry
{
	class CDegree;

	class CRadian
	{
	public:
		explicit CRadian ( double r = 0.0 ) : _radian(r) {}
		CRadian ( const CDegree& d );
		CRadian(const CRadian& r) : _radian(r._radian){}
		CRadian& operator = ( const double& f ) { _radian = f; return *this; }
		CRadian& operator = ( const CRadian& r ) { _radian = r._radian; return *this; }
		CRadian& operator = ( const CDegree& d );

		double ValueDegrees() const { return _radian / RadianPI * 180.0;}
		double ValueRadians() const { return _radian; }

		const CRadian& operator + () const { return *this; }
		CRadian operator + ( const CRadian& r ) const { return CRadian ( _radian + r._radian ); }
		CRadian operator + ( const CDegree& d ) const;
		CRadian& operator += ( const CRadian& r ) { _radian += r._radian; return *this; }
		CRadian& operator += ( const CDegree& d );
		CRadian operator - () const { return CRadian(-_radian); }
		CRadian operator - ( const CRadian& r ) const { return CRadian ( _radian - r._radian ); }
		CRadian operator - ( const CDegree& d ) const;
		CRadian& operator -= ( const CRadian& r ) { _radian -= r._radian; return *this; }
		CRadian& operator -= ( const CDegree& d );
		CRadian operator * ( double f ) const { return CRadian ( _radian * f ); }
		CRadian operator * ( const CRadian& f ) const { return CRadian ( _radian * f._radian ); }
		CRadian& operator *= ( double f ) { _radian *= f; return *this; }
		CRadian operator / ( double f ) const { return CRadian ( _radian / f ); }
		CRadian& operator /= ( double f ) { _radian /= f; return *this; }

		bool operator <  ( const CRadian& r ) const { return _radian <  r._radian; }
		bool operator <= ( const CRadian& r ) const { return _radian <= r._radian; }
		bool operator == ( const CRadian& r ) const { return _radian == r._radian; }
		bool operator != ( const CRadian& r ) const { return _radian != r._radian; }
		bool operator >= ( const CRadian& r ) const { return _radian >= r._radian; }
		bool operator >  ( const CRadian& r ) const { return _radian >  r._radian; }

	private:
		double _radian;
	};

	class CDegree
	{
	public:
		explicit CDegree ( double d = 0.0 ) : _degree(d) {}
		CDegree(const CDegree& d) : _degree(d._degree) {}
		CDegree(const CRadian& r) : _degree(r.ValueDegrees()) {}
		CDegree& operator = ( const double& f ) { _degree = f; return *this; }
		CDegree& operator = ( const CDegree& d ) { _degree = d._degree; return *this; }
		CDegree& operator = ( const CRadian& r ) { _degree = r.ValueDegrees(); return *this; }

		double ValueDegrees() const { return _degree; }
		double ValueRadians() const { return _degree / 180.0 * RadianPI;}

		const CDegree& operator + () const { return *this; }
		CDegree operator + ( const CDegree& d ) const { return CDegree ( _degree + d._degree ); }
		CDegree operator + ( const CRadian& r ) const { return CDegree ( _degree + r.ValueDegrees() ); }
		CDegree& operator += ( const CDegree& d ) { _degree += d._degree; return *this; }
		CDegree& operator += ( const CRadian& r ) { _degree += r.ValueDegrees(); return *this; }
		CDegree operator - () const { return CDegree(-_degree); }
		CDegree operator - ( const CDegree& d ) const { return CDegree ( _degree - d._degree ); }
		CDegree operator - ( const CRadian& r ) const { return CDegree ( _degree - r.ValueDegrees() ); }
		CDegree& operator -= ( const CDegree& d ) { _degree -= d._degree; return *this; }
		CDegree& operator -= ( const CRadian& r ) { _degree -= r.ValueDegrees(); return *this; }
		CDegree operator * ( double f ) const { return CDegree ( _degree * f ); }
		CDegree operator * ( const CDegree& f ) const { return CDegree ( _degree * f._degree ); }
		CDegree& operator *= ( double f ) { _degree *= f; return *this; }
		CDegree operator / ( double f ) const { return CDegree ( _degree / f ); }
		CDegree& operator /= ( double f ) { _degree /= f; return *this; }

		bool operator <  ( const CDegree& d ) const { return _degree <  d._degree; }
		bool operator <= ( const CDegree& d ) const { return _degree <= d._degree; }
		bool operator == ( const CDegree& d ) const { return _degree == d._degree; }
		bool operator != ( const CDegree& d ) const { return _degree != d._degree; }
		bool operator >= ( const CDegree& d ) const { return _degree >= d._degree; }
		bool operator >  ( const CDegree& d ) const { return _degree >  d._degree; }

	private:
		double _degree; 
	};

	inline CRadian::CRadian ( const CDegree& d ) : _radian(d.ValueRadians()) {}

	inline CRadian& CRadian::operator = ( const CDegree& d ) 
	{
		_radian = d.ValueRadians();
		return *this;
	}

	inline CRadian CRadian::operator + ( const CDegree& d ) const 
	{
		return CRadian ( _radian + d.ValueRadians() );
	}

	inline CRadian& CRadian::operator += ( const CDegree& d ) 
	{
		_radian += d.ValueRadians();
		return *this;
	}

	inline CRadian CRadian::operator - ( const CDegree& d ) const 
	{
		return CRadian ( _radian - d.ValueRadians() );
	}

	inline CRadian& CRadian::operator -= ( const CDegree& d ) 
	{
		_radian -= d.ValueRadians();
		return *this;
	}

	inline CRadian operator * ( double a, const CRadian& b )
	{
		return CRadian ( a * b.ValueRadians() );
	}

	inline CRadian operator / ( double a, const CRadian& b )
	{
		return CRadian ( a / b.ValueRadians() );
	}

	inline CDegree operator * ( double a, const CDegree& b )
	{
		return CDegree ( a * b.ValueDegrees() );
	}

	inline CDegree operator / ( double a, const CDegree& b )
	{
		return CDegree ( a / b.ValueDegrees() );
	}
}

#endif // Angle_Degree_Radian_h__
