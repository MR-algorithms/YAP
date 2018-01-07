#ifndef CMATRIX44_H
#define CMATRIX44_H
#include "geometry.h"

class CMatrix44
{
public:
    CMatrix44();
    CMatrix44(const CMatrix44& rhs);
    union
    {
        struct
        {
            double _00, _01, _02, _03;
            double _10, _11, _12, _13;
            double _20, _21, _22, _23;
            double _30, _31, _32, _33;
        };
        double _elements[16];  //!< the each element in the phalanx
        double _m[4][4];
    };

   static CMatrix44 FromBasisVectors( const Vector3D& orient0, const Vector3D& orient1, const Vector3D& orient2);
   CMatrix44 GetTranspose() const;
   static CMatrix44 Translate(const Vector3D& offset);

   friend CMatrix44 operator * (const CMatrix44& u, const CMatrix44& v);
   friend Point3D operator * (const CMatrix44& matrix, const Point3D& point);
private:
    void Unit();
    void MakeTranslate(const Vector3D& point);

};

#endif // CMATRIX44_H
