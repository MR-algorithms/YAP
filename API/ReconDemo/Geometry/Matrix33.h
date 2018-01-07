#ifndef CMATRIX33_H
#define CMATRIX33_H

#include "geometry.h"

namespace Geometry
{
	class CRadian;

	class CMatrix33
	{
	public:
		union
		{
			struct
			{
				double _00, _01, _02;
				double _10, _11, _12;
				double _20, _21, _22;
			};
			double _elements[9];  //!< the each element in the phalanx
			double _m[3][3];
		};

	public:
		CMatrix33(void);
		CMatrix33(const CMatrix33& rhs);
		~CMatrix33(void);

		CMatrix33 GetTranspose() const;
		bool GetInverse(CMatrix33& inverse) const;

		CMatrix33& operator=(const CMatrix33& matrix);
		CMatrix33& operator*=(const CMatrix33& matrix);
		
		static CMatrix33 Rotate(const CRadian& r);
		static CMatrix33 Translate(double x, double y);
		static CMatrix33 Scale(double fx, double fy);
		static CMatrix33 HFlip();
		static CMatrix33 VFlip();

		void MakeRotate(const CRadian& r);
        void MakeTranslate(const Vector2D& point);
        void MakeScale(const Vector2D& scale);

		// Building a Matrix33 from translate * rotation * scale.
        static CMatrix33 FromTransform(const Vector2D& translate, const CRadian& r, const Vector2D& scale);
        static CMatrix33 FromBasisVectors(const Vector2D& orient0, const Vector2D& orient1);

		// Decompose a Matrix33 to translate * rotation * scale.
		//static void DecomposeMatrix(const CMatrix33& matrix, CSize2D<double>& translate, CRadian& rotation, CSize2D<double>& scale);

        friend Vector2D  operator * (const CMatrix33& matrix, const Vector2D& vec);
        friend Point2D operator * (const CMatrix33& matrix, const Point2D& point);
		friend CMatrix33 operator * (const CMatrix33& u, const CMatrix33& v);

	private:
		void Unit();
	};
}
#endif
